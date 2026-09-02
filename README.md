# Multivoid verified findings

This branch is a public staging set for independently reviewed findings. Each file in `findings/` contains one finding. A reviewer can accept or reject each file without changing another finding.

The status values have these meanings:

- `REPORTED`: A report exists. No controlled measurement confirms the reported behavior.
- `PARTIALLY_VERIFIED`: A controlled measurement confirms part of the report. A required condition or action is missing.
- `VERIFIED`: Controlled evidence confirms the stated behavior under the stated conditions.
- `FIXED`: A change exists and a controlled test confirms the change.
- `CLOSED`: The finding needs no further action. The finding records the reason.

The cause-strength values have these meanings:

- `MEASURED`: Direct source or runtime evidence proves the stated cause.
- `INFERRED`: Measured facts and a stated assumption support the cause.
- `HYPOTHESIS`: The cause is a testable proposal with incomplete support.
- `UNKNOWN`: The available evidence does not identify the cause.

A finding in this set is not an upstream issue report. No upstream issue or pull request has been opened for these findings.
