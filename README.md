# Efficient-Path-Finder

A program written for unimelb COMP10002 subject project in semester 2

## Function

This programs takes a test files that contains the grid map in a certain format and it 
returns the most efficient path to take with it costs. It also draw up 
the map of all most efficient path to each point in the map to take from the starting point.

This code applies Depth First Search Algorithm to find the most efficient path.

## Test Files Format

The test files first lines will contain the size of the grid. <br />
E.g 3 3 means a 3x3 grid map <br />
<br />
The next lines will contain the cost of the path from each intersection <br />
E.g 2b 999 1 1 1 <br />
It means from intersection 2b it will cost 999 to go west, 1 to go north, 1 to go east, and 1 to go south. <br /> 
999 means that it's impossible to go through that path<br />
<br />
The next lines after the intersection cost info is the starting point and followed by the destination points. <br />
