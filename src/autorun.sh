#!/bin/bash

while true; do
./server >/dev/null

if [ $? -eq 1 ]
then
  exit 0
fi
sleep 120
done
