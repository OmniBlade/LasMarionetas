#include <gtest/gtest.h>
#include <rz/rzcmdline.h>
#include <string.h>

TEST(commandline, cmdline_parse)
{
    const char *argv[] = { "testcmdline.exe", "-moomins", "-testing", "file1", "-m", "black socks" };
    int argc = 6;
    const char cmdline[] = "testcmdline -clangers -releasing file2 -c \"white socks\"";
    cRZString argtest("testcmdline.exe -moomins -testing file1 -m \"black socks\"");
    const char *cmdtest[] = { "testcmdline", "-clangers", "-releasing", "file2", "-c", "white socks" };
    cRZCmdLine arg(argc, const_cast<char **>(argv));
    cRZCmdLine cmd(cmdline);
    cRZString tmp;
    arg.GetCommandLineText(tmp);

    EXPECT_EQ(arg.ArgC(), argc);
    EXPECT_EQ(cmd.ArgC(), argc);
    EXPECT_EQ(tmp, argtest);

    for (int i = 0; i < cmd.ArgC(); ++i) {
        EXPECT_STREQ(cmd[i].c_str(), cmdtest[i]);
    }
 
    EXPECT_EQ(arg.IsSwitchPresent(cRZString("moomins")), true);
    EXPECT_EQ(arg.GetIndexOfSwitch(cRZString("moomins"), 0), 1);
    EXPECT_EQ(cmd.IsSwitchPresent(cRZString("clangers")), true);
    EXPECT_EQ(cmd.GetIndexOfSwitch(cRZString("clangers"), 0), 1);
    EXPECT_EQ(arg.IsSwitchPresent('m', tmp, true), true);
    EXPECT_EQ(arg.GetIndexOfSwitch('m', 0), 4);
    EXPECT_EQ(cmd.IsSwitchPresent('c', tmp, true), true);
    EXPECT_EQ(cmd.GetIndexOfSwitch('c', 0), 4);
}
