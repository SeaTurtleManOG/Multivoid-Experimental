# tests/standalone/run.ps1 -- compile and run the standalone candidate tests.
#
# These tests are single translation units against the real headers; they need no stub tree
# and no CMake configure, which is why they can run on a machine where the full build cannot.
#
# Every candidate_*.cpp in this directory is registered below. It has to be: a candidate file that
# sits here unregistered is a fix nobody tested, and a green run of this suite would say nothing
# at all about it. So "the file list and the case list agree" is a property to keep, not a
# one-time correction, and it is checked mechanically: every
# Invoke-Case records its test source, and after the last case the run fails unless the recorded
# set equals the candidate_*.cpp files in this directory (REGISTRATION line).
#
# Every registered candidate other than candidate_01 carries an __has_include fallback holding
# the PRE-FIX policy (the comment above each case names the header it hides),
# so one source measures both sides of its fix. That only means something if both sides are
# actually built, so each of them runs twice: once with the fix header on the include path, once
# with that header removed from a copy of the include tree. The second arm MUST fail -- a pre-fix
# arm that passes says the assertion does not discriminate, and the script treats that as an error
# rather than a pass.
#
# candidate_01 carries no such fallback (its fix is inside ue_wrap/core/call.cpp, which the test
# links, not behind a policy header), so it runs one arm. Its pass is therefore unfalsified here.
#
# Success is the STANDALONE_CANDIDATES_PASS line AND exit code 0.

$ErrorActionPreference = 'Stop'

$TestRoot = $PSScriptRoot
$SourceRoot = [IO.Path]::GetFullPath((Join-Path $TestRoot '..\..'))
$IncludeRoot = Join-Path $SourceRoot 'include'

$VsWhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
if (!(Test-Path -LiteralPath $VsWhere)) {
    throw "Visual Studio locator not found: $VsWhere"
}
$VsInstall = (& $VsWhere -latest -products * `
    -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    -property installationPath | Select-Object -First 1)
if (!$VsInstall) { throw 'No Visual Studio C++ toolchain was found' }
$VcVars = Join-Path $VsInstall 'VC\Auxiliary\Build\vcvars64.bat'
if (!(Test-Path -LiteralPath $VcVars)) { throw "vcvars64.bat not found: $VcVars" }

$TempBase = [IO.Path]::GetFullPath([IO.Path]::GetTempPath())
$TempRoot = [IO.Path]::GetFullPath((Join-Path $TempBase `
    ("multivoid-standalone-{0}" -f [Guid]::NewGuid().ToString('N'))))
if (!$TempRoot.StartsWith($TempBase, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing temp path outside temp root: $TempRoot"
}
New-Item -ItemType Directory -Path $TempRoot | Out-Null

function Invoke-Compile([string]$Name, [string]$IncludeDir, [string[]]$Sources) {
    $BuildRoot = Join-Path $TempRoot $Name
    New-Item -ItemType Directory -Path $BuildRoot | Out-Null
    $Exe = Join-Path $BuildRoot "$Name.exe"
    $Pdb = Join-Path $BuildRoot "$Name.pdb"
    $Quoted = ($Sources | ForEach-Object { '"{0}"' -f $_ }) -join ' '
    $OldPath = $env:PATH
    try {
        $env:PATH = "$(Split-Path -Parent $VsWhere);$OldPath"
        $Command = 'call "{0}" >nul && cl /nologo /std:c++20 /EHsc /W4 /WX ' +
            '/I"{1}" /Fo"{2}\\" /Fd"{3}" /Fe"{4}" {5}'
        $Command = $Command -f $VcVars, $IncludeDir, $BuildRoot, $Pdb, $Exe, $Quoted
        $CompileOutput = & cmd.exe /d /c $Command
        $CompileOutput | ForEach-Object { Write-Host $_ }
        if ($LASTEXITCODE -ne 0) { throw "compile '$Name' failed with exit $LASTEXITCODE" }
    } finally {
        $env:PATH = $OldPath
    }
    return $Exe
}

# A copy of the include tree with the named headers removed, so __has_include selects the test's
# own pre-fix fallback. Each header must be PRESENT before it is removed: if it were already
# absent the two arms would be the same compile and the negative would prove nothing.
function New-PreFixIncludeRoot([string]$Name, [string[]]$Hide) {
    $Root = Join-Path $TempRoot "include-$Name"
    New-Item -ItemType Directory -Path $Root | Out-Null
    Copy-Item -Path (Join-Path $IncludeRoot '*') -Destination $Root -Recurse -Force
    # The copy must be the whole tree, or a "pre-fix failure" could just be a missing header.
    $Source = (Get-ChildItem -LiteralPath $IncludeRoot -Recurse -File).Count
    $Copied = (Get-ChildItem -LiteralPath $Root -Recurse -File).Count
    if ($Copied -ne $Source) { throw "include copy is incomplete: $Copied of $Source files" }
    foreach ($Rel in $Hide) {
        if (!(Test-Path -LiteralPath (Join-Path $IncludeRoot $Rel))) {
            throw "$Rel is absent from the include tree: the fix arm is not the fixed arm"
        }
        $Path = Join-Path $Root $Rel
        if (!(Test-Path -LiteralPath $Path)) { throw "nothing to hide at $Rel in the copy" }
        Remove-Item -LiteralPath $Path -Force
        if (Test-Path -LiteralPath $Path) { throw "failed to hide $Rel" }
    }
    return [PSCustomObject]@{
        Root = $Root
        Note = "PREFIX_ARM name=$Name files=$Copied/$Source hidden=$($Hide -join ',')"
    }
}

$Registered = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)

function Invoke-Case([string]$Name, [string]$IncludeDir, [string[]]$Sources, [string]$Expect) {
    [void]$Registered.Add((Split-Path -Leaf $Sources[0]))
    $Exe = Invoke-Compile $Name $IncludeDir $Sources
    Write-Output "=== $Name (expect $Expect) ==="
    & $Exe
    $Exit = $LASTEXITCODE
    if ($Expect -eq 'pass') {
        if ($Exit -ne 0) { throw "'$Name' failed with exit $Exit but was expected to pass" }
        Write-Output "EXPECTED_PASS name=$Name exit=$Exit"
    } elseif ($Expect -eq 'fail') {
        if ($Exit -eq 0) {
            throw "'$Name' passed, but a pre-fix arm that passes shows the assertion does not " +
                  'discriminate between the pre-fix and fixed policy'
        }
        Write-Output "EXPECTED_FAIL name=$Name exit=$Exit"
    } else {
        throw "unknown expectation '$Expect'"
    }
}

$Ok = $false
try {
    $Call = Join-Path $SourceRoot 'src\ue_wrap\core\call.cpp'
    if (!(Test-Path -LiteralPath $Call)) { throw "call.cpp not found: $Call" }

    # candidate_01 -- the fix is in call.cpp, which this test links; one arm only.
    Invoke-Case 'candidate_01-fixed' $IncludeRoot `
        @((Join-Path $TestRoot 'candidate_01_param_frame_boundary.cpp'), $Call) 'pass'

    # candidate_08 -- both arms across coop/props/place_queue_admission.h. The pre-fix fallback
    # tests capacity alone, ahead of any eligibility sample, so eligibility is not a term in the
    # decision: that is what the fixed policy changed and what the pre-fix arm must fail on.
    Invoke-Case 'candidate_08-fixed' $IncludeRoot `
        @((Join-Path $TestRoot 'candidate_08_place_queue_admission.cpp')) 'pass'
    $Pre08 = New-PreFixIncludeRoot 'candidate_08' @('coop\props\place_queue_admission.h')
    Write-Output $Pre08.Note
    Invoke-Case 'candidate_08-prefix' $Pre08.Root `
        @((Join-Path $TestRoot 'candidate_08_place_queue_admission.cpp')) 'fail'

    # candidate_12 -- both arms across coop/props/drive_place_authorship.h. The pre-fix fallback is
    # `isDriveClass && freshBirth`, which is what the note's old position inside `if (freshBirth)`
    # amounted to.
    Invoke-Case 'candidate_12-fixed' $IncludeRoot `
        @((Join-Path $TestRoot 'candidate_12_drive_place_authorship.cpp')) 'pass'
    $Pre12 = New-PreFixIncludeRoot 'candidate_12' @('coop\props\drive_place_authorship.h')
    Write-Output $Pre12.Note
    Invoke-Case 'candidate_12-prefix' $Pre12.Root `
        @((Join-Path $TestRoot 'candidate_12_drive_place_authorship.cpp')) 'fail'

    # CASES-END (each integration step appends its candidates above this line)

    # The file list and the case list must agree: a candidate source nobody registered is
    # a fix nobody tested, and a registered source that is gone is a case that tests nothing.
    $OnDisk = @(Get-ChildItem -LiteralPath $TestRoot -Filter 'candidate_*.cpp' -File |
        ForEach-Object { $_.Name } | Sort-Object)
    $Unregistered = @($OnDisk | Where-Object { !$Registered.Contains($_) })
    $Missing = @($Registered | Where-Object { $OnDisk -notcontains $_ })
    Write-Output ("REGISTRATION on_disk={0} registered={1} unregistered={2} missing={3}" -f `
        $OnDisk.Count, $Registered.Count, ($Unregistered -join ','), ($Missing -join ','))
    if ($Unregistered.Count -ne 0 -or $Missing.Count -ne 0) {
        throw 'the candidate files and the registered cases disagree'
    }

    Write-Output 'STANDALONE_CANDIDATES_PASS'
    $Ok = $true
} finally {
    if (Test-Path -LiteralPath $TempRoot) {
        $Resolved = [IO.Path]::GetFullPath($TempRoot)
        if (!$Resolved.StartsWith($TempBase, [StringComparison]::OrdinalIgnoreCase)) {
            throw "Refusing cleanup outside temp root: $Resolved"
        }
        Remove-Item -LiteralPath $Resolved -Recurse -Force
    }
}

# The last thing run was a negative control's exe, so $LASTEXITCODE is its 1. Say what this
# SCRIPT concluded instead of leaking that.
if ($Ok) { exit 0 } else { exit 1 }
