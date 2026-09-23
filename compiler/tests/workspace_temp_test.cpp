#include "ao/Compiler.hpp"

#include <gtest/gtest.h>

#include <string>

using ao::compiler::CompileEnv;
using ao::compiler::compileMethod;
using ao::compiler::disassemble;
using ao::compiler::LitKind;

namespace {

CompileEnv workspaceEnv() {
  CompileEnv env;
  env.undeclaredAreBindings = true;
  env.knownGlobals = {"Object"};
  return env;
}

// The number of Binding literals named `name`.
int bindingLiterals(const ao::compiler::MethodImage& image, const std::string& name) {
  int n = 0;
  for (const auto& lit : image.literals) {
    if (lit.kind == LitKind::Binding && lit.text == name) {
      ++n;
    }
  }
  return n;
}

}  // namespace

// SPEC §3.10: 未定義名は束縛。代入は PopStoreLitVar、読みは PushLitVar で、temp を使わない。
TEST(WorkspaceBinding, UndeclaredAssignIsBindingStore) {
  const char* src =
      "doIt\n"
      "  x := 4.\n"
      "  ^x";
  auto r = compileMethod(src, workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(0, r.image.numTemps);
  EXPECT_EQ(1, bindingLiterals(r.image, "x"));
  const std::string d = disassemble(r.image);
  EXPECT_NE(std::string::npos, d.find("PopStoreLitVar"));
  EXPECT_NE(std::string::npos, d.find("PushLitVar"));
  EXPECT_EQ(std::string::npos, d.find("Temp"));
}

TEST(WorkspaceBinding, KnownGlobalAssignIsError) {
  auto r = compileMethod("doIt\n  Object := 1", workspaceEnv());
  EXPECT_FALSE(r.ok);
  EXPECT_NE(std::string::npos, r.error.message.find("cannot assign"));
}

TEST(WorkspaceBinding, KnownGlobalReadIsPushGlobal) {
  auto r = compileMethod("doIt\n  ^Object", workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_NE(std::string::npos, disassemble(r.image).find("PushGlobal"));
  EXPECT_EQ(0, bindingLiterals(r.image, "Object"));
}

// ブロックの中の代入も同じ束縛に書く（コピーしない）。
TEST(WorkspaceBinding, BlockAssignmentIsBindingStore) {
  const char* src =
      "doIt\n"
      "  [ x := 2 ] value.\n"
      "  ^x";
  auto r = compileMethod(src, workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(0, r.image.numTemps);
  EXPECT_EQ(1, bindingLiterals(r.image, "x"));
  const std::string d = disassemble(r.image);
  EXPECT_NE(std::string::npos, d.find("CreateBlock 0 0"));
  const ao::compiler::MethodImage* inner = nullptr;
  for (const auto& lit : r.image.literals) {
    if (lit.kind == LitKind::Method) {
      inner = lit.method.get();
    }
  }
  ASSERT_NE(nullptr, inner);
  EXPECT_EQ(1, bindingLiterals(*inner, "x"));
  EXPECT_NE(std::string::npos, disassemble(*inner).find("StoreLitVar 0"));
}

// 00 High: 宣言した temp は同じ名前の束縛と関係しない。
TEST(WorkspaceBinding, DeclaredTempIgnoresBinding) {
  const char* src =
      "doIt\n"
      "  | q |\n"
      "  ^q";
  auto r = compileMethod(src, workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(1, r.image.numTemps);
  EXPECT_EQ(0, bindingLiterals(r.image, "q"));
  EXPECT_NE(std::string::npos, disassemble(r.image).find("PushTemp 0"));
}

// 束縛は temp の 255 に数えない。
TEST(WorkspaceBinding, ManyBindingsNeedNoTemps) {
  std::string src = "doIt\n";
  for (int i = 0; i < 250; ++i) {
    src += "  v" + std::to_string(i) + " := nil.\n";
  }
  auto r = compileMethod(src, workspaceEnv());
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_EQ(0, r.image.numTemps);
}

// 束縛を使わないメソッドでは、未定義名はこれまでどおり PushGlobal。
TEST(WorkspaceBinding, MethodUndeclaredIsGlobal) {
  auto r = compileMethod("foo\n  ^Zork");
  ASSERT_TRUE(r.ok) << r.error.message;
  EXPECT_NE(std::string::npos, disassemble(r.image).find("PushGlobal"));
  EXPECT_EQ(0, bindingLiterals(r.image, "Zork"));
}
