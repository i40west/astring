# astring
Implements a string buffer that can be appended endlessly, and will reallocate its
memory as needed to fit the newly appended strings.

This is pretty straightforward. An astring is a string that you can append to as
much as you want, and it will reallocate memory for you as you go.

The idea of releasing it is that, if you're in a position to use it, you understand
the code and don't have to care if I maintain it or not. You could have written it.
Here it is; you don't have to.

See the header file for how to use it.

