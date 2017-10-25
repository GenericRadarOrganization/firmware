# Data Acquisition/Processing Procedure
This document outlines the desired process for data handling in the receive path

1. Capture data for a period of time - save off to a buffer(preferably using DMA to write to the buffer)
2. Process saved data
   1. Scan to find median
   2. Look for zero crossings
   3. Compute avg. period from zero crossings
   4. Compute speed from period?
3. Publish data w/ timestamp to public struct
4. Another task processes published data and displays it
