#!/bin/bash

echo "scale=2; -45 + 175*$(cat /sys/class/sensor/sht31/temperature)/65535" | bc
echo "scale=2; 100*$(cat /sys/class/sensor/sht31/humidity)/65535" | bc

