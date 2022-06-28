# System Security

## file security

Best practices:

1. open / read /write / close with user specific / group specific access.
2. restrict flags: user and group and mostly none for others.
3. restrict exec flags on normal files: open file with `0660`.
4. restrict read / write operation on binaries: give only `0100` or 0110` etc.
