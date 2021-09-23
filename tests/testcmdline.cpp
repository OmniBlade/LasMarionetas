#include <rz/rzcmdline.h>
#include <string.h>

const char *argv[] = { "testcmdline.exe", "-moomins", "-testing", "file1", "-m", "black socks" };
int argc = 6;
const char cmdline[] = "testcmdline -clangers -releasing file2 -c \"white socks\"";

cRZString argtest("testcmdline.exe -moomins -testing file1 -m \"black socks\"");
const char *cmdtest[] = { "testcmdline", "-clangers", "-releasing", "file2", "-c", "white socks" };

int test_cmdline()
{
    int ret = 0;
    cRZCmdLine arg(argc, const_cast<char **>(argv));
    cRZCmdLine cmd(cmdline);

    if (arg.ArgC() != 6 || cmd.ArgC() != 6) {
        fprintf(stderr, "ArgC for synthetic command lines does not equal 6! arg: %d, cmd : %d\n", arg.ArgC(), cmd.ArgC());
        ret = 1;
    }

    cRZString arg_cmdline;
    arg.GetCommandLineText(arg_cmdline);
    if (arg_cmdline != argtest) {
        fprintf(stderr,
            "arg command line string not as expected:\n        got: '%s'\n   expected: '%s'\n",
            arg_cmdline.c_str(),
            argtest.c_str());
        ret = 1;
    }

    for (int i = 0; i < cmd.ArgC(); ++i) {
        if (strcmp(cmd[i].c_str(), cmdtest[i]) != 0) {
            fprintf(stderr,
                "cmd command line string not as expected at %d:\n        got: '%s'\n   expected: '%s'\n",
                i,
                cmd[i].c_str(),
                cmdtest[i]);
            ret = 1;
        }
    }

    if (!arg.IsSwitchPresent(cRZString("moomins")) || arg.GetIndexOfSwitch(cRZString("moomins"), 0) != 1) {
        fprintf(stderr,
            "arg command line switch 'moomins' not as expected at index 1:\n        present: '%s'\n   index: %d\n",
            arg.IsSwitchPresent(cRZString("moomins")) ? "true" : "false",
            arg.GetIndexOfSwitch(cRZString("moomins"), 0));
        ret = 1;
    }

    if (!cmd.IsSwitchPresent(cRZString("clangers")) || cmd.GetIndexOfSwitch(cRZString("clangers"), 0) != 1) {
        fprintf(stderr,
            "cmd command line switch 'clangers' not as expected at index 1:\n        present: '%s'\n   index: %d\n",
            cmd.IsSwitchPresent(cRZString("clangers")) ? "true" : "false",
            cmd.GetIndexOfSwitch(cRZString("clangers"), 0));
        ret = 1;
    }

    cRZString tmp;

    if (!arg.IsSwitchPresent('m', tmp, true) || arg.GetIndexOfSwitch('m', 0) != 4)
        {
        fprintf(stderr,
            "arg command line switch 'm' not as expected at index 4:\n        present: '%s'\n   index: %d\n",
            arg.IsSwitchPresent('m', tmp, true) ? "true" : "false",
            arg.GetIndexOfSwitch('m', 0));
        ret = 1;
    }

    if (!cmd.IsSwitchPresent('c', tmp, true) || cmd.GetIndexOfSwitch('c', 0) != 4) {
        fprintf(stderr,
            "cmd command line switch 'c' not as expected at index 4:\n        present: '%s'\n   index: %d\n",
            cmd.IsSwitchPresent('c', tmp, true) ? "true" : "false",
            cmd.GetIndexOfSwitch('c', 0));
        ret = 1;
    }

    return ret;
}

int main(int argc, char **argv)
{
    int ret = 0;

    ret |= test_cmdline();

    return ret;
}
