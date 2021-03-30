#include "test.h"
#include "result.h"

Result_float divide(float a, float b) {
    if (b == 0) {
        Result_float err = Err(b, "Division by 0 is not allowed");
        return err;
    } else {
        return Ok(a / b);
    }
}