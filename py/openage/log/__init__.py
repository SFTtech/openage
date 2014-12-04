# Copyright 2014-2014 the openage authors. See copying.md for legal info.

import os
import os.path

dbgstack = [(None, 0)]
verbose = 0


def set_verbosity(newlevel):
    global verbose
    verbose = newlevel


def ifdbg(lvl):
    global verbose

    if verbose >= lvl:
        return True
    else:
        return False


def ifdbgeq(lvl):
    global verbose

    if verbose == lvl:
        return True
    else:
        return False


def dbg(msg=None, lvl=None, push=None, pop=None, lazymsg=None, end="\n"):
    """
    msg
        message to print.
        str() will be called on this object.
        if None, the logger state is changed according to the other args,
        but no message is printed.
    lvl
        integer level. if None, the loglevel remains unchanged.
    push
        if not None, will be pushed onto the debug stack.
        the current level is pushed onto the stack as well, for restoring
        on pop.

        messages are indented according to the debug stack length.
    pop
        if not None, the top element of the debug stack will be poped.
        the old loglevel is restored.
        an exception is generated if the object is not equal to the top
        object on the debug stack.
    lazymsg
        can be used instead of msg for lazy evaluation.
        must be callable, and return a message object (or None).
        will be called only if the message will actually be evaluated
        and printed (according to loglevel).
        intended for use with lambdas.
        only one of msg and lazymsg may be not None.
    end
        the 'end' argument to print
    """

    global verbose

    if lvl is None:
        # if no level is set, use the level on top of the debug stack
        lvl = dbgstack[-1][1]

    if pop is not None:
        if pop is True:
            if dbgstack.pop()[0] is None:
                raise Exception("stack underflow in debug stack!")
        elif dbgstack.pop()[0] != pop:
            raise Exception(str(pop) + " is not on top of the debug stack")

    if verbose >= lvl:
        if lazymsg is not None:
            if callable(lazymsg):
                msg = lazymsg()
            else:
                raise Exception("the lazy message must be a callable (lambda)")

        if msg is not None:
            print((len(dbgstack) - 1) * "  " + str(msg), end=end)

    if push is not None:
        dbgstack.append((push, lvl))
