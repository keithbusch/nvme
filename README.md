This is almost a total rewrite of nvme-cli.

Consistent output format, help documentation, and keeping up with the
latest features and trends are some things that separate successful
programs from unsucessful ones. While nvme-cli makes a good effort at
that, I find adding new commands, options, or structures are incredibly
boring and entirely tedious tasks.

This project's goal is to retain the existing functionality, but make it
easier to update by heavily abusing macros to generate self reflecting
structures and command parsing.

Finally, we'd like to abstract the OS layer and run the same nvme tool
on as many operating systems as possible.

The current state of this project is very much a work-in-progress,
and may remain so indefinitely.
