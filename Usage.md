Make sure you go through [Installation](Installation.md) steps before reading this article.

Once you have everything set up, simply launch the application as usual to start tracking gdi/user handles.
The client library will check for any active allocations at program termination and issue a debug break if any allocations are still active. Meaning, you'll need a debugger attached prior to application termination in order to able to view allocation statistics.

Note, though, that it is hardly a 100% indication of a handle leak - some libraries might not have been unloaded and still hold handles in their caches (I know Qt does, and so do many others).

The same goes for regular allocation statistics checks.
Assuming you have a debugger attached, have it break in and issue the following command to display statistics:

> `!leakdbg.handle -stats`

_Although you could use any debugger implementation that's part of the Windows Debugging Tools (cdb, windbg or ntsd), since the extension is using debugger's markup language, it is best to use windbg.exe to minimize the typing (it will hyperlink most of its output for better navigation experience)_.

Issuing `!leakdbg.help` will display help describing all the supported operations.
Here's a brief description of the commands:

  * use `!leakdbg.handle -list` to get the list of all active allocations
  * `!leakdbg.handle -h <handle>` will display allocation stack for a particular handle value
  * use `!leakdbg.handle -snapshot` to take a snapshot of the currently active allocations
  * use `!leakdbg.handle -stats -diff` to display statistics compared to the prevously taken snapshot

Snapshots can be saved to a file whenever running under a debugger causes a performance degradation:

> `!leakdbg.handle -snapshot -write yourapp.sht`

and re-read at a later point:

> `!leakdbg.handle -snapshot -read yourapp.sht`

The special `-v` verbose flag is self-describing. The flag can be used with most of the commands to enhance their output.

Whenever you feel you're getting way too many false positives (some allocations in what's referred to as system modules are pretty valid and are constantly causing "leaks"), you might want to start a so-called supressions filter file where you could describe multiple suppression rules for those allocations you wish to ignore:

> `!leakdbg.handle -filter -read windowsxp.flt`

The supression rules file format is very simple and naive:
```
^systemmodule!functionname
*systemmodule2!functionname2
^mymodule!functionname3
```
It basically describes functions that when matched on the allocation stack would prevent the debugger extension from including them in statistics (you can still have all allocations displayed by issuing a command with an `-ignore` switch).

`^` denotes a function on the top of the allocation stack, while `*` denotes a function that can be found anywhere on the allocation stack (simple optimization step since `^` rules are processed first).

Following iterative approach is the most common use of the framework:

  * start your application, break in with the debugger and take an initial snapshot before any serious resource allocations take place.
  * let the application run through a few allocation/release loops.
  * compare the currently active allocations with the previous snapshot.
  * make corrections to the suppression rules file.
  * have your application run a few loops more.
  * make a comparison, rule out impossibles, finalize your analysis by fixing the leaks ;o)