param(
    [switch]$SkipOmissions
)

$ErrorActionPreference = 'Stop'

$TestRoot = $PSScriptRoot
$SourceRoot = [IO.Path]::GetFullPath((Join-Path $TestRoot '..\..'))
$RepoRoot = [IO.Path]::GetFullPath((Join-Path $SourceRoot '..\..'))
$IncludeRoot = Join-Path $SourceRoot 'include'
$StubRoot = Join-Path $TestRoot 'stubs'
$Harness = Join-Path $TestRoot 'qualification.cpp'
$BacklogSource = Join-Path $SourceRoot 'src\coop\net\send_backlog.cpp'
# The negative control's pre-fix image. $Pinned is UPSTREAM_BASE -- the commit this branch's
# candidate commits sit directly on -- so the pinned header is the target's OWN pre-fix header:
# the control then measures the fix, not the distance to some other tree.
#
# Pin history, because this line has now gone stale twice: f690c017 (previous upstream main,
# orphaned by a force-rewrite of that branch), then 3af5ddae (the rewritten main, since advanced).
# 3af5ddae..b7bd09db fast-forwards, and its delta on THIS header is a comment rewrap only, so the
# 3af5ddae pin was still substantively measuring the right thing -- it was provenance-stale, not
# wrong. It went stale silently because the two blob guards below constrain the FILE, never the
# pin. PINNED_BASE_IS_PREFIX, added alongside them, is what now makes a stale pin fail loudly.
# b7bd09db then stayed the pin through a8437658. These changes are rebuilt
# on upstream ba3bf74a (protocol 162), so the pin is that commit; interactable_channel.h is
# unchanged between a8437658 and ba3bf74a, and PINNED_BASE_IS_PREFIX re-proves it on this line.
$Pinned = 'ba3bf74aec84a0e28705af14bb1ca31260021567'
$HeaderRel = 'src/votv-coop/include/coop/interactables/interactable_channel.h'

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
    ("multivoid-baseline-delivery-{0}" -f [Guid]::NewGuid().ToString('N'))))
if (!$TempRoot.StartsWith($TempBase, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing temp path outside temp root: $TempRoot"
}
New-Item -ItemType Directory -Path $TempRoot | Out-Null

function Invoke-Compile([string]$Name, [string]$HeaderRoot) {
    $BuildRoot = Join-Path $TempRoot $Name
    New-Item -ItemType Directory -Path $BuildRoot | Out-Null
    $Exe = Join-Path $BuildRoot 'qualification.exe'
    $Pdb = Join-Path $BuildRoot 'qualification.pdb'
    $OldPath = $env:PATH
    try {
        $env:PATH = "$(Split-Path -Parent $VsWhere);$OldPath"
        $Command = 'call "{0}" >nul && cl /nologo /std:c++20 /EHsc /W4 /WX ' +
            '/I"{1}" /I"{2}" /I"{3}" /Fo"{4}\\" /Fd"{5}" /Fe"{6}" "{7}" "{8}"'
        $Command = $Command -f $VcVars, $StubRoot, $HeaderRoot, $IncludeRoot,
            $BuildRoot, $Pdb, $Exe, $Harness, $BacklogSource
        $CompileOutput = & cmd.exe /d /c $Command
        $CompileOutput | ForEach-Object { Write-Host $_ }
        if ($LASTEXITCODE -ne 0) { throw "compile '$Name' failed with exit $LASTEXITCODE" }
    } finally {
        $env:PATH = $OldPath
    }
    return $Exe
}

function Invoke-MustPass([string]$Name, [string]$Exe, [string[]]$Arguments) {
    Write-Output "=== PASS CONTROL: $Name ==="
    & $Exe @Arguments
    if ($LASTEXITCODE -ne 0) { throw "'$Name' failed with exit $LASTEXITCODE" }
}

function Invoke-MustFail([string]$Name, [string]$Exe, [string[]]$Arguments) {
    Write-Output "=== EXPECTED NEGATIVE: $Name ==="
    & $Exe @Arguments
    $Exit = $LASTEXITCODE
    if ($Exit -eq 0) { throw "'$Name' unexpectedly passed" }
    Write-Output "EXPECTED_FAIL name=$Name exit=$Exit"
}

function Invoke-Git([string[]]$Arguments) {
    $Output = & git.exe -C $RepoRoot @Arguments
    if ($LASTEXITCODE -ne 0) { throw "git $($Arguments -join ' ') failed with exit $LASTEXITCODE" }
    return ($Output | Select-Object -First 1).Trim()
}

function Write-GitBlob([string]$Spec, [string]$Destination) {
    $Parent = Split-Path -Parent $Destination
    New-Item -ItemType Directory -Force -Path $Parent | Out-Null
    $Info = [Diagnostics.ProcessStartInfo]::new()
    $Info.FileName = 'git.exe'
    $Info.WorkingDirectory = $RepoRoot
    $Info.UseShellExecute = $false
    $Info.RedirectStandardOutput = $true
    $Info.RedirectStandardError = $true
    $Info.ArgumentList.Add('cat-file')
    $Info.ArgumentList.Add('blob')
    $Info.ArgumentList.Add($Spec)
    $Process = [Diagnostics.Process]::Start($Info)
    $File = [IO.File]::Open($Destination, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write)
    try {
        $Process.StandardOutput.BaseStream.CopyTo($File)
    } finally {
        $File.Dispose()
    }
    $ErrorText = $Process.StandardError.ReadToEnd()
    $Process.WaitForExit()
    if ($Process.ExitCode -ne 0) {
        throw "git cat-file failed for '$Spec': $ErrorText"
    }
}

function Replace-Required([string]$Text, [string]$Old, [string]$New) {
    if (!$Text.Contains($Old)) { throw "mutation anchor not found: $Old" }
    return $Text.Replace($Old, $New)
}

function New-MutatedHeader([string]$Name, [scriptblock]$Mutator) {
    $Root = Join-Path $TempRoot "header-$Name"
    $Destination = Join-Path $Root 'coop\interactables\interactable_channel.h'
    New-Item -ItemType Directory -Path (Split-Path -Parent $Destination) | Out-Null
    $Text = Get-Content -Raw (Join-Path $IncludeRoot 'coop\interactables\interactable_channel.h')
    $Mutated = & $Mutator $Text
    [IO.File]::WriteAllText($Destination, $Mutated, [Text.UTF8Encoding]::new($false))
    return $Root
}

$Ok = $false
try {
    # The complete fixed suite is deliberately first. Every negative executable
    # also runs its two instrumentation controls before the selected assertion.
    $CurrentExe = Invoke-Compile 'current' $IncludeRoot
    Invoke-MustPass 'fixed complete suite' $CurrentExe @()

    $PinnedRoot = Join-Path $TempRoot 'header-pinned'
    $PinnedHeader = Join-Path $PinnedRoot 'coop\interactables\interactable_channel.h'
    Write-GitBlob "$Pinned`:$HeaderRel" $PinnedHeader
    # A negative control means nothing unless it was built from the pre-fix bytes. Two checks: the
    # file on disk IS the pinned blob, and it is NOT the header the fixed arm just compiled. The
    # second is what catches a stale extraction or an include order that reached past $PinnedRoot.
    $WantBlob = Invoke-Git @('rev-parse', "$Pinned`:$HeaderRel")
    $GotBlob = Invoke-Git @('hash-object', $PinnedHeader)
    $FixedBlob = Invoke-Git @('hash-object',
        (Join-Path $IncludeRoot 'coop\interactables\interactable_channel.h'))
    Write-Output "PINNED_HEADER pinned=$Pinned want=$WantBlob got=$GotBlob fixed=$FixedBlob"
    if ($GotBlob -ne $WantBlob) {
        throw "pinned header is not ${Pinned}'s blob: $GotBlob != $WantBlob"
    }
    if ($GotBlob -eq $FixedBlob) {
        throw 'pinned header equals the fixed header: the negative control would assert nothing'
    }
    # Those two constrain the FILE but not the pin: ANY commit whose header differs from the fixed
    # one satisfies both, which is exactly how $Pinned sat one upstream generation stale and still
    # passed. So derive the pre-fix image instead of trusting the pin -- the header's pre-fix
    # content is its content at the PARENT of the newest commit on HEAD that touched it -- and
    # require the pin to agree. This is the check that fails when the pin is no longer the base.
    # It assumes that newest toucher IS the fix under test; if an unrelated later commit touches
    # the header it fires spuriously, which is the intended failure direction -- loud, with both
    # hashes named, never a silent pass.
    $FixCommit = Invoke-Git @('rev-list', '-1', 'HEAD', '--', $HeaderRel)
    $DerivedBlob = Invoke-Git @('rev-parse', ('{0}^:{1}' -f $FixCommit, $HeaderRel))
    Write-Output "PINNED_BASE_IS_PREFIX fix=$FixCommit derived=$DerivedBlob pinned=$WantBlob"
    if ($DerivedBlob -ne $WantBlob) {
        throw ("pinned base $Pinned is not this candidate's pre-fix image: ${FixCommit}^ gives " +
            "$DerivedBlob but the pin gives $WantBlob -- repoint `$Pinned at the real base")
    }
    # The blob checks above cannot tell two commits apart whose header bytes agree, and this header
    # was byte-identical from b7bd09db through a8437658 to ba3bf74a -- so on this line a stale
    # b7bd09db pin would pass every one of them. Prove the pin at commit level too: the newest
    # upstream commit this HEAD contains, merge-base(HEAD, upstream/main), must BE the pin. A
    # missing upstream ref is a failure, not a skip. The proof is against the LOCAL remote-tracking
    # ref: a ref that lags behind HEAD's real base can pass a stale pin, so the line prints the
    # ref's own commit and the ref must be fetched to at least the base for the PASS to mean it.
    $UpstreamRef = 'refs/remotes/upstream/main'
    $MergeBase = Invoke-Git @('merge-base', 'HEAD', $UpstreamRef)
    $UpstreamTip = Invoke-Git @('rev-parse', $UpstreamRef)
    Write-Output "PINNED_IS_UPSTREAM_BASE pinned=$Pinned merge_base=$MergeBase ref=$UpstreamRef ref_tip=$UpstreamTip"
    if ($MergeBase -ne $Pinned) {
        throw "pinned base $Pinned is not merge-base(HEAD, $UpstreamRef) = $MergeBase"
    }
    $PinnedExe = Invoke-Compile 'pinned' $PinnedRoot
    Invoke-MustFail 'pinned header lacks targeted retry' $PinnedExe @('--case', 'retry')

    if (!$SkipOmissions) {
        $Cases = @(
            @{
                Name = 'omit-pending-enqueue'; Case = 'retry';
                Mutate = {
                    param($t)
                    Replace-Required $t `
                        '                QueuePendingDelivery(peerSlot, peerGeneration, d.first);' `
                        '                // intentional omission: no pending enqueue'
                }
            },
            @{
                Name = 'omit-retry-worker'; Case = 'retry';
                Mutate = {
                    param($t)
                    Replace-Required $t '            RetryPendingDeliveries(s);' `
                        '            // intentional omission: retry worker disabled'
                }
            },
            @{
                Name = 'omit-generation-guard'; Case = 'slot-reuse';
                Mutate = {
                    param($t)
                    $t = Replace-Required $t '        CancelStalePendingDeliveries(s);' `
                        '        // intentional omission: no eager generation pruning'
                    $t = Replace-Required $t `
                        'if (s->peerGenerationForSlot(it->slot) != it->peerGeneration)' 'if (false)'
                    $Pattern = 's->SendReliableToSlotForGeneration\(it->slot, it->peerGeneration,\s+a_\.kind'
                    $Mutated = [regex]::Replace($t, $Pattern, 's->SendReliableToSlot(it->slot, a_.kind')
                    if ($Mutated -eq $t) { throw 'generation-conditioned retry anchor not found' }
                    $Mutated
                }
            },
            @{
                Name = 'omit-current-state-read'; Case = 'current-state';
                Mutate = {
                    param($t)
                    $Pattern = '            bool on = false;\r?\n            if \(!a_\.ReadState\(actor, on\)\) \{\r?\n                \+\+it;\r?\n                continue;\r?\n            \}'
                    $Mutated = [regex]::Replace($t, $Pattern,
                        '            const bool on = true;  // intentional omission: stale queued ON')
                    if ($Mutated -eq $t) { throw 'current-state read anchor not found' }
                    $Mutated
                }
            },
            @{
                Name = 'omit-snapshot-success-bank'; Case = 'success-bank';
                Mutate = {
                    param($t)
                    Replace-Required $t `
                        '                { std::lock_guard<std::mutex> lk(stateMutex_); lastKnown_[d.first] = on; }' `
                        '                // intentional omission: accepted snapshot not banked'
                }
            },
            @{
                Name = 'omit-retry-success-bank'; Case = 'current-state';
                Mutate = {
                    param($t)
                    Replace-Required $t `
                        '                { std::lock_guard<std::mutex> lk(stateMutex_); lastKnown_[it->key] = on; }' `
                        '                // intentional omission: accepted retry not banked'
                }
            },
            @{
                Name = 'omit-peer-left-cancel'; Case = 'lifecycle';
                Mutate = {
                    param($t)
                    Replace-Required $t '        CancelPendingDeliveriesForSlot(slot);' `
                        '        // intentional omission: peer-left cancellation disabled'
                }
            },
            @{
                Name = 'omit-disconnect-cancel'; Case = 'lifecycle';
                Mutate = {
                    param($t)
                    Replace-Required $t '        pendingDeliveries_.clear();' `
                        '        // intentional omission: disconnect cancellation disabled'
                }
            },
            @{
                Name = 'omit-missing-actor-cancel'; Case = 'missing-actor';
                Mutate = {
                    param($t)
                    $Old = @'
            if (!actor) {
                it = pendingDeliveries_.erase(it);
                ++cancelled;
                continue;
            }
'@
                    $New = @'
            if (!actor) {
                ++it;  // intentional omission: retain a row whose actor is missing
                continue;
            }
'@
                    Replace-Required $t $Old $New
                }
            },
            @{
                Name = 'omit-truthful-accounting'; Case = 'accounting';
                Mutate = {
                    param($t)
                    Replace-Required $t 'connect-snapshot -- attempted=%d accepted=' `
                        'connect-snapshot -- sent %d full state(s); accepted='
                }
            }
        )

        foreach ($Entry in $Cases) {
            $HeaderRoot = New-MutatedHeader $Entry.Name $Entry.Mutate
            $Exe = Invoke-Compile $Entry.Name $HeaderRoot
            Invoke-MustFail $Entry.Name $Exe @('--case', $Entry.Case)
        }
    }

    Write-Output 'QUALIFICATION_AND_OMISSION_CASES_PASS'
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

# On the success path the last native command run was a negative control's exe, whose exit code
# this script REQUIRES to be nonzero (Invoke-MustFail). Ending here without an `exit` left that
# nonzero standing in $LASTEXITCODE, so a caller that dot-sources or `& `-invokes this script --
# the pass/fail idiom this file itself uses at the Invoke-* sites above -- read a PASSING run as a
# failure. Say what this SCRIPT concluded instead of leaking the last exe's code.
# This does not swallow anything: every genuine failure above is a throw under
# $ErrorActionPreference = 'Stop', which leaves $Ok false and terminates with a nonzero code.
# Placed after `finally` so the temp-tree cleanup has already run. Matches tests/standalone/run.ps1.
if ($Ok) { exit 0 } else { exit 1 }
