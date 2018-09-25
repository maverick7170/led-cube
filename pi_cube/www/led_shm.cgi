#!/bin/bash
echo "Content-Type: text/html"
echo ""
echo ""
echo $QUERY_STRING | sudo tee /dev/shm/control_shm
