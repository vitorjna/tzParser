# tzParser
Merge timezone information in different formats
Made in Qt6

Reads timezone information in the format Region/City and DST change rule in Posix, and merges it with the country name by cross checking the Region/City for each country.

The result is also parsed into columns containing the DST change rules and offsets, to make the final result easier to store in a DB.


Not a complete approach, but may be useful for some cases.
