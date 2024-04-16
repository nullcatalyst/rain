#include "rain/spec/util.hpp"

TEST(Interface, static_definition_only) {
    const std::string_view code = R"(
interface Closable {
    fn close()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Interface, instance_definition_only) {
    const std::string_view code = R"(
interface Closable {
    fn close(&self)
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Interface, implementation) {
    const std::string_view code = R"(
struct Player {
    x: f32,
    y: f32,
}

interface Entity {
    fn update(&self)
}

//impl Player : Entity {
//    fn update(&self) {
//        self.x = self.x + 1.0
//        self.y = self.y + 1.0
//    }
//}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
