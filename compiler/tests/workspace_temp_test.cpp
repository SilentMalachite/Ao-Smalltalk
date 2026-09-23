#include "ao/Compiler.hpp"

#include <gtest/gtest.h>

#include <string>

using ao::compiler::CompileEnv;
using ao::compiler::compileMethod;
using ao::compiler::disassemble;

CompileEnv workspaceEnv() {
  CompileEnv env;
  env.undeclaredAreTemps = true;
  env.knownGlobals = {"Object"};
  env.workspaceTemps = {};
  return env;
}

TEST(WorkspaceTemp, UndeclaredAssignIsWorkspaceTemp) {
  const char* src =
      "doIt\n"
      "  x := 4.\n"
      "  ^x";
  auto r = compileMethod(src, workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(1u, r.image.tempBindings.size());
  EXPECT_EQ("x", r.image.tempBindings[0].name);
  EXPECT_TRUE(r.image.tempBindings[0].workspace);
  EXPECT_EQ(1, r.image.numTemps);
}

TEST(WorkspaceTemp, KnownGlobalAssignIsError) {
  auto r = compileMethod("doIt\n  Object := 1", workspaceEnv());
  EXPECT_FALSE(r.ok);
  EXPECT_NE(std::string::npos, r.error.message.find("cannot assign"));
}

TEST(WorkspaceTemp, KnownGlobalReadIsPushGlobal) {
  auto r = compileMethod("doIt\n  ^Object", workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_NE(std::string::npos, disassemble(r.image).find("PushGlobal"));
}

TEST(WorkspaceTemp, BlockCaptureIsMethodTemp) {
  const char* src =
      "doIt\n"
      "  [ x := 2 ] value.\n"
      "  ^x";
  auto r = compileMethod(src, workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(1u, r.image.tempBindings.size());
  EXPECT_EQ("x", r.image.tempBindings[0].name);
  EXPECT_TRUE(r.image.tempBindings[0].workspace);
  EXPECT_NE(std::string::npos, disassemble(r.image).find("CreateBlock"));
}

TEST(WorkspaceTemp, PreseededDeclaredAndCapturedKeepOrder) {
  CompileEnv env;
  env.undeclaredAreTemps = true;
  env.workspaceTemps = {"b", "a"};
  const char* src =
      "doIt\n"
      "  | t a |\n"
      "  t := 1.\n"
      "  z := 2.\n"
      "  ^a";
  auto r = compileMethod(src, env);
  ASSERT_TRUE(r.ok) << r.error.message;
  ASSERT_EQ(4u, r.image.tempBindings.size());
  EXPECT_EQ("b", r.image.tempBindings[0].name);
  EXPECT_TRUE(r.image.tempBindings[0].workspace);
  EXPECT_EQ("a", r.image.tempBindings[1].name);
  EXPECT_TRUE(r.image.tempBindings[1].workspace);
  EXPECT_EQ("t", r.image.tempBindings[2].name);
  EXPECT_FALSE(r.image.tempBindings[2].workspace);
  EXPECT_EQ("z", r.image.tempBindings[3].name);
  EXPECT_TRUE(r.image.tempBindings[3].workspace);
  EXPECT_EQ(4, r.image.numTemps);
}
