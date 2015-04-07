# Design/Implementation overviews #

For those interested to know how things are implemented before they skip through the source code.

The implementation has been kept very simple. It is a poor man's effective tool at tracking leaks. The primary target for the framework have been the GDI/USER handle leaks but it can be extended to support any other types of leaks.

One of the grounds I laid prior to coding has been the avoidance of the C pre-processor. That's why I settled on a simple code generator written in JScript (Microsoft's Java Script interpreter).
I also opted for maximal code/tool reuse. Which means I only conceived the internal data structures used to keep stats on resource allocations. I did not bother myself with either code injection (courtesy of Detours library) or IPC (implemented via remote access from debugger directly into the debugged target's memory). This means there's as much to a fancy GUI as you can get with `windbg`. Not much of a problem - I assure you, after having successfully nailed down most of the elusive resource leaks in the code base of a product I'm currently working on, I can only recommend it.

The weakest link in the whole picture, in my opinion, is the dependency on the exact version of the compiler's STL (version 1.0 supports STL from VC71) as a result of a design decision to rely on std::hash\_map<> data structure to store resource allocations.

Well, implementing a leak tracker is easy. It is even easier the more you think about it. With all the tools at your disposal, nothing seems impossible. The harder problem is evaluating information such a leak tracker will present you with. Which are false positives? Which are cached resources? Why would you miss a resource release?

These problems kept creeping at me once I started the development.
Well, for starters, I figured, in `gdi32.dll` world, would it not be nice to suppress allocations which are caused by other higher-level APIs from the same `gdi` namespace?
Which I implemented by setting a nested\_call flag at the beginning of the allocation routine and clearing it at its end. Any allocations that happen as a result of a current call thus get suppressed. Is it a good thing? For some allocations, sure. But I would not go as far as enabling it for everything else (especially `user32`, which indeed invokes user code and its user code I was intended to bulletproof in the first place).
How about unobvious API allocation rules? For instance, child windows (`CreateWindow`) get implicitly removed when you `DestroyWindow` a parent hwnd.
Or take `LoadImage` for example which creates 3 distinct types of GDI/USER objects:
  * bitmap (deleted with `DeleteObject`)
  * cursor (deleted with `DestroyCursor`)
  * icon (deleted with `DestroyIcon`)

Having given it a bit of thought, I decided to factor the cleanup decision rules into a separate entity I named `cleanup_callback` which is responsible for determining which allocation corresponds to a particular type of a release.

Also, I figured catching errors of allocation/release mismatch would be a great thing.
The tracker will warn you about such cases by issuing a debug break each time a violation is detected and a debugger is currently attached. Sometimes, though, resources are composite, consisting of little lower-level parts - and it bites back giving me debug breaks at wrong places (_this issue has not yet been addressed_).