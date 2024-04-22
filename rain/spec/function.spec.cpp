#include "rain/spec/util.hpp"

TEST(Function, definition) {
    const std::string_view code = R"(
export fn four() -> i32 {
    4
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, calling_another) {
    const std::string_view code = R"(
export fn four() -> i32 {
    4
}

export fn double_four() -> i32 {
    2 * four()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, out_of_order_definition) {
    const std::string_view code = R"(
export fn double_four() -> i32 {
    2 * four()
}

export fn four() -> i32 {
    4
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, nested) {
    const std::string_view code = R"(
export fn double_four() -> i32 {
    fn four() -> i32 {
        4
    }

    2 * four()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, call_static_method) {
    const std::string_view code = R"(
fn f32x4.zero() -> f32x4 {
    f32x4 {
        x: 0.0,
        y: 0.0,
        z: 0.0,
        w: 0.0,
    }
}

export fn call_static_method() -> f32 {
    f32x4.zero().y
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, if_else) {
    const std::string_view code = R"(
export fn conditional() -> i32 {
    if true {
        2 * 4
    } else {
        2 * 5
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, if_else_if) {
    const std::string_view code = R"(
export fn conditional() -> i32 {
    if false {
        2 * 4
    } else if true {
        2 * 5
    } else {
        2 * 6
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, fib_iteration) {
    const std::string_view code = R"(
export fn fib_iteration(n: i32) -> i32 {
    let a = 0
    let b = 1

    let n = n
    while n > 0 {
        let tmp = a
        a = b
        b = tmp + b
        n = n - 1
    }

    a
}

#define DO_PRINT 1
#define DO_OPTIMIZE 1

TEST(Demo, demo) {
    const std::string_view code = R"(
fn f32x4.zero() -> f32x4 {
    f32x4 {
        x: 0.0,
        y: 0.0,
        z: 0.0,
        w: 0.0,
    }
}

fn f32x4.splat(v: f32) -> f32x4 {
    f32x4 {
        x: v,
        y: v,
        z: v,
        w: v,
    }
}

fn f32x4.new(x: f32, y: f32, z: f32, w: f32) -> f32x4 {
    f32x4 {
        x: x,
        y: y,
        z: z,
        w: w,
    }
}

struct Input {
    left: bool,
    right: bool,
    up: bool,
    down: bool,
    jump: bool,
}

let pressed_buttons = Input{ left: false, right: false, up: false, down: false, jump: false }
let buttons = Input{ left: false, right: false, up: false, down: false, jump: false }

fn Input.reset(&self) {
    self.left = false
    self.right = false
    self.up = false
    self.down = false
    self.jump = false
}

fn Input.press(&self, key: i32) {
    if key == 37 {
        self.left = true
    } else if key == 39 {
        self.right = true
    } else if key == 38 {
        self.up = true
    } else if key == 40 {
        self.down = true
    } else if key == 32 {
        self.jump = true
    }
}

fn Input.release(&self, key: i32) {
    if key == 37 {
        self.left = false
    } else if key == 39 {
        self.right = false
    } else if key == 38 {
        self.up = false
    } else if key == 40 {
        self.down = false
    } else if key == 32 {
        self.jump = false
    }
}

struct Player {
    position: f32x4,
    velocity: f32x4,
    standing: bool,
}

fn Player.new() -> Player {
    Player {
        position: f32x4.zero(),
        velocity: f32x4.zero(),
        standing: false,
    }
}

fn Player.update(&self) {
    if buttons.left {
        self.velocity.x = self.velocity.x - 0.1
    }
    if buttons.right {
        self.velocity.x = self.velocity.x + 0.1
    }

    if pressed_buttons.jump {
        if self.standing {
            self.velocity.y = -1.0
            self.standing = false
            null
        }
    }

    self.position = self.position + self.velocity

    if self.position.y < 0.0 {
        self.position.y = 0.0
        self.velocity.y = 0.0
        self.standing = true
        null
    }
}

let player = Player.new()

export fn keyboard(key: i32, pressed: bool) {
    if pressed {
        buttons.press(key)
        pressed_buttons.press(key)
    } else {
        buttons.release(key)
    }
}

export fn next_frame() {
    player.update()
    pressed_buttons.reset()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
