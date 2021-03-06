#ifndef TESTINGTESTS_H
#define	TESTINGTESTS_H

#include "Test.h"

class TestingTests : public Test {
public:
    TestingTests() : Test("Testing the unit test suite") {

    }

    virtual void run() {

        if(!assertEquals("zzzz", "zzzz", 4)) {
            trace("assertEquals with given length broken");
        }

        if(!assertEquals("aaaa", "aaaa")) {
            trace("assertEquals broken");
        }

        if(!assertFalse(false)) {
            trace("assertFalse broken");
        }

        if(!assertTrue(true)) {
            trace("assertTrue broken");
        }
    }
};

#endif	/* TESTINGTESTS_H */

