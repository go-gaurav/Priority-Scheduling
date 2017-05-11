# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(priority-donate-condvar) begin
(priority-donate-condvar) Thread Low : creating thread
(priority-donate-condvar) Thread Med : creating thread
(priority-donate-condvar) Thread High: creating thread
(priority-donate-condvar) Thread Main: signalling...
(priority-donate-condvar) Thread Low : is awake.
(priority-donate-condvar) Thread High: is awake.
(priority-donate-condvar) Thread High: finished.
(priority-donate-condvar) Thread Med : is awake.
(priority-donate-condvar) Thread Med : finished.
(priority-donate-condvar) Thread Low : finished.
(priority-donate-condvar) Thread Main: finished.
(priority-donate-condvar) end
EOF
pass;
