## wlcopy is a Wayland clipboard utility
---
This tool can copy text/images on any (i think) Wayland compositor

### Flags:
-f - run in foreground (doesn't fork)
-p - paste the clipboard content only once
(Run it like `wlcopy [-f -p]` with pipelining into the wlcopy)
It takes input from STDIN


### Usage examples
```
echo "TEXT" | wlcopy
echo "TEXT" | wlcopy -f
cat file.txt | wlcopy
grim -g "$(slurp)" - | wlcopy
