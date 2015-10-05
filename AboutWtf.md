# Introduction #

**wtf** is a simple utility which allows you automatically rename your existing files (or create hard links) using names specified in torrent file.

If you have some content (a set of files) and wish to join to existing seeding on some BitTorrent tracker, you cannot do this while files in torrent file differs from files you have in names.

You can edit the torrent file, but you still cannot join seeding, since names affect info hash of the torrent file (tracker would not be able to recognize this torrent or would recognize it as different torrent).

There are two solutions to this problem. The first is to rename your files. And the second is to create hard links (alternative name for existing file).

This utility will help you to do this automatically.

See also:
  * [using Process Single](ProcessSingleSUCcess.md)
  * [how to rename (or create hard links) using the utility](RenameSUCcess.md)