#ifndef CATCH_CONFIG_MAIN
#  define CATCH_CONFIG_MAIN
#endif

#include "atm.hpp"
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
//                             Helper Definitions //
/////////////////////////////////////////////////////////////////////////////////////////////

bool CompareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1);
  std::ifstream f2(p2);

  if (f1.fail() || f2.fail()) {
    return false;  // file problem
  }

  std::string f1_read;
  std::string f2_read;
  while (f1.good() || f2.good()) {
    f1 >> f1_read;
    f2 >> f2_read;
    if (f1_read != f2_read || (f1.good() && !f2.good()) ||
        (!f1.good() && f2.good()))
      return false;
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Test Cases
/////////////////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Example: Create a new account", "[ex-1]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto accounts = atm.GetAccounts();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);

  Account sam_account = accounts[{12345678, 1234}];
  REQUIRE(sam_account.owner_name == "Sam Sepiol");
  REQUIRE(sam_account.balance == 300.30);

  auto transactions = atm.GetTransactions();
  REQUIRE(accounts.contains({12345678, 1234}));
  REQUIRE(accounts.size() == 1);
  std::vector<std::string> empty;
  REQUIRE(transactions[{12345678, 1234}] == empty);
}

TEST_CASE("Example: Simple widthdraw", "[ex-2]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  atm.WithdrawCash(12345678, 1234, 20);
  auto accounts = atm.GetAccounts();
  Account sam_account = accounts[{12345678, 1234}];

  REQUIRE(sam_account.balance == 280.30);
}

TEST_CASE("Example: Print Prompt Ledger", "[ex-3]") {
  Atm atm;
  atm.RegisterAccount(12345678, 1234, "Sam Sepiol", 300.30);
  auto& transactions = atm.GetTransactions();
  transactions[{12345678, 1234}].push_back(
      "Withdrawal - Amount: $200.40, Updated Balance: $99.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $40000.00, Updated Balance: $40099.90");
  transactions[{12345678, 1234}].push_back(
      "Deposit - Amount: $32000.00, Updated Balance: $72099.90");
  atm.PrintLedger("./prompt.txt", 12345678, 1234);
  REQUIRE(CompareFiles("./ex-1.txt", "./prompt.txt"));
}
TEST_CASE("RegisterAccount should reject duplicate accounts", "[register]") {
  Atm atm;
  atm.RegisterAccount(1111, 2222, "Alice", 100.0);
  REQUIRE_THROWS_AS(atm.RegisterAccount(1111, 2222, "Alice", 200.0), std::invalid_argument);
}

TEST_CASE("WithdrawCash should reject negative and excessive withdrawals", "[withdraw]") {
  Atm atm;
  atm.RegisterAccount(1234, 5678, "Bob", 50.0);
  REQUIRE_THROWS_AS(atm.WithdrawCash(1234, 5678, -10.0), std::invalid_argument);
  REQUIRE_THROWS_AS(atm.WithdrawCash(1234, 5678, 100.0), std::runtime_error);
  atm.WithdrawCash(1234, 5678, 20.0);
  REQUIRE(atm.CheckBalance(1234, 5678) == Approx(30.0));
}

TEST_CASE("DepositCash should reject negative deposits", "[deposit]") {
  Atm atm;
  atm.RegisterAccount(2222, 3333, "Charlie", 0.0);

  REQUIRE_THROWS_AS(atm.DepositCash(2222, 3333, -50.0), std::invalid_argument);

  atm.DepositCash(2222, 3333, 100.0);
  REQUIRE(atm.CheckBalance(2222, 3333) == Approx(100.0));
}

  TEST_CASE("PrintLedger generates correct ledger", "[ledger]") {
  Atm atm;
  atm.RegisterAccount(11112222, 2222, "Alice", 1000.0);
  atm.DepositCash(11112222, 2222, 500.0);
  atm.WithdrawCash(11112222, 2222, 200.0);

  atm.PrintLedger("./ledger_out.txt", 11112222, 2222);


  REQUIRE(CompareFiles("./ledger_expected.txt", "./ledger_out.txt"));
}

