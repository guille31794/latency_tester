#!/bin/bash
# Start a virtual framebuffer and VNC server for GUI applications.
# Connect with any VNC client to localhost:5900

echo ">> Starting Xvfb (virtual framebuffer 800x480)..."
Xvfb :0 -screen 0 800x480x24 &
sleep 1

echo ">> Starting x11vnc (VNC server on port 5900)..."
x11vnc -display :0 -forever -nopw -listen 0.0.0.0 -rfbport 5900 &
sleep 1

export DISPLAY=:0
echo ">> Environment ready. Connect VNC client to localhost:5900"
echo ">> Run your application with: ./LatencyTester"
echo ""

# Execute whatever command was passed (default: bash)
exec "$@"
