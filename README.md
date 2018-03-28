# Satellite
Der Code für den Satelliten

##Requirements:
qbcan lib (little optimized on our fork)
tiny gps

##Concept:
* Cloudlink1: Sensor part, collects all the data and sends them to Cloudlink2 and Groundstation.
* Cloudlink2: Logs data to SD, due to SD logging consuming about 35% RAM of an Arduino Nano
