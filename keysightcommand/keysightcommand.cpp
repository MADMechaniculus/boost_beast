#include "keysightcommand.h"

using namespace KeysightSCPI;

std::map<uint16_t, std::string> keysightCommand::signalSINFields = {
    {0, "channel"},
    {1, "freq"},
    {2, "voltH"},
    {3, "voltL"},
    {4, "phase"},
};

std::map<uint16_t, std::string> keysightCommand::signalSQUFields = {
    {0, "channel"},
    {1, "dcyc"},
    {2, "freq"},
    {3, "voltH"},
    {4, "voltL"},
};

std::map<uint16_t, std::string> keysightCommand::signalRampFields = {
    {0, "channel"},
    {1, "symmetry"},
    {2, "freq"},
    {3, "voltH"},
    {4, "voltL"},
};

std::map<uint16_t, std::string> keysightCommand::signalPulsFields = {
    {0, "channel"},
    {1, "lead"},
    {2, "trail"},
    {3, "width"},
    {4, "freq"},
    {5, "voltH"},
    {6, "voltL"},
};

std::map<uint16_t, std::string> keysightCommand::signalTRIFields = {
    {0, "channel"},
    {1, "freq"},
    {2, "voltH"},
    {3, "voltL"},
    {4, "phase"},
};

std::map<uint16_t, std::string> keysightCommand::signalNOISFields = {
    {0, "channel"},
    {1, "voltH"},
    {2, "voltL"},
    {3, "band"},
};

std::map<uint16_t, std::string> keysightCommand::signalPRBSFields = {
    {0, "channel"},
    {1, "brate"},
    {2, "voltH"},
    {3, "voltL"},
};

std::map<uint16_t, std::string> keysightCommand::signalDCFields = {
    {0, "channel"},
    {1, "offset"},
};

keysightCommand::keysightCommand()
{

}

std::string keysightCommand::signalErr()
{
    std::string command = ":SYST:ERR?\n";
    return command;
}

bool keysightCommand::signalSIN(int output, double freq, double voltH, double voltL, double phase, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(output) + ":FUNC SIN\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FREQuency " + std::to_string(freq) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":PHASe " + std::to_string(phase) + "\n");
    command.push_back("OUTP" + std::to_string(output) + " ON\n");

    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalSINFields.at(2) + ", " + signalSINFields.at(3);
        return false;
    }
    if (!checkFreq(freq,"SIN",voltH-voltL)) {
        wrongParam += signalSINFields.at(1);
        return false;
    }
    if (!checkPhase(phase)) {
        wrongParam += signalSINFields.at(4);
        return false;
    }

    return true;
}

bool keysightCommand::signalSQU(int output, double dcyc, double freq, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(output) + ":FUNC SQU\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FREQuency " + std::to_string(freq) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FUNC:SQU:DCYC " + std::to_string(dcyc) + "\n");
    command.push_back("OUTP" + std::to_string(output) + " ON\n");

    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalSQUFields.at(3) + ", " + signalSQUFields.at(4);
        return false;
    }
    if (!checkFreq(freq,"SQU",voltH-voltL)) {
        wrongParam += signalSQUFields.at(2);
        return false;
    }
    if (!checkDcycle(dcyc)) {
        wrongParam += signalSQUFields.at(1);
        return false;
    }
    return true;
}

bool keysightCommand::signalRamp(int output, double symmetry, double freq, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(output) + ":FUNC RAMP\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FREQuency " + std::to_string(freq) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FUNC:RAMP:SYMMetry " + std::to_string(symmetry) + "\n");
    command.push_back("OUTP" + std::to_string(output) + " ON\n");

    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalRampFields.at(3) + ", " + signalRampFields.at(4);
        return false;
    }
    if (!checkFreq(freq,"RAMP",voltH-voltL)) {
        wrongParam += signalRampFields.at(2);
        return false;
    }
    if (!checkSymmetry(symmetry)) {
        wrongParam += signalRampFields.at(1);
        return false;
    }
    return true;
}

bool keysightCommand::signalPuls(int output, double lead, double trail, double width, double freq, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(output) + ":FUNC PULS\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FREQuency " + std::to_string(freq) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FUNC:PULS:WIDT " + std::to_string(width) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FUNC:PULS:TRAN:LEAD " + std::to_string(lead) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FUNC:PULS:TRAN:TRA " + std::to_string(trail) + "\n");
    command.push_back("OUTP" + std::to_string(output) + " ON\n");

    double amp = voltH-voltL;
    double period = 1/freq;
    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalPulsFields.at(5) + ", " + signalPulsFields.at(6);
        return false;
    }
    if (!checkFreq(freq,"PULS",amp)) {
        wrongParam += signalPulsFields.at(4);
        return false;
    }
    if (!checkWidth(width,amp,period)) {
        wrongParam += signalPulsFields.at(3);
        return false;
    }
    if (!checkFrontInRange(lead,amp)) {
        wrongParam += signalPulsFields.at(1);
        return false;
    }
    if (!checkFrontInRange(trail,amp)) {
        wrongParam += signalPulsFields.at(2);
        return false;
    }
    if (!checkFrontConflict(lead,trail,width,period)) {
        wrongParam += signalPulsFields.at(1) + ", " + signalPulsFields.at(2);
        return false;
    }
    return true;
}

bool keysightCommand::signalARB(std::string file, std::list<std::string> & command)
{
    command.push_back("FUNCtion:ARBitrary \"INT:\\BUILTIN\\" + file + "\"\n");
    command.push_back("FUNCtion ARB\n");

    return true;
}

bool keysightCommand::signalTRI(int output, double freq, double voltH, double voltL, double phase, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(output) + ":FUNC TRI\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":FREQuency " + std::to_string(freq) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":PHASe " + std::to_string(phase) + "\n");
    command.push_back("OUTP" + std::to_string(output) + " ON\n");

    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalTRIFields.at(2) + ", " + signalTRIFields.at(3);
        return false;
    }
    if (!checkFreq(freq,"TRI",voltH-voltL)) {
        wrongParam += signalTRIFields.at(1);
        return false;
    }
    if (!checkPhase(phase)) {
        wrongParam += signalTRIFields.at(4);
        return false;
    }
    return true;
}

bool keysightCommand::signalNOIS(int output, double voltH, double voltL, double band, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(output) + ":FUNC NOIS\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(output) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back(":FUNC:NOISe:BWIDth " + std::to_string(band) + "\n");
    command.push_back("OUTP" + std::to_string(output) + " ON\n");

    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalNOISFields.at(1) + ", " + signalNOISFields.at(2);
        return false;
    }
    if (!checkBand(band,voltH-voltL)) {
        wrongParam += signalNOISFields.at(3);
        return false;
    }
    return true;
}

bool keysightCommand::signalPRBS(int channel, double brate, double voltH, double voltL, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(channel) + ":FUNC PRBS\n");
    command.push_back("SOUR" + std::to_string(channel) + ":VOLTage:HIGH " + std::to_string(voltH) + "\n");
    command.push_back("SOUR" + std::to_string(channel) + ":VOLTage:LOW " + std::to_string(voltL) + "\n");
    command.push_back("SOUR" + std::to_string(channel) + ":FUNCtion:PRBS:BRATe " + std::to_string(brate) + "\n");
    command.push_back("OUTP" + std::to_string(channel) + " ON\n");

    wrongParam = "";
    if (!checkVolt(voltH,voltL)) {
        wrongParam += signalPRBSFields.at(2) + ", " + signalPRBSFields.at(3);
        return false;
    }
    if (!checkBrate(brate,voltH-voltL)) {
        wrongParam += signalPRBSFields.at(1);
        return false;
    }
    return true;
}

bool keysightCommand::signalDC(int channel, double offset, std::list<std::string> & command, std::string & wrongParam)
{
    command.push_back("SOUR" + std::to_string(channel) + ":FUNC DC\n");
    command.push_back("SOUR" + std::to_string(channel) + ":VOLTage:OFFSet " + std::to_string(offset) + "\n");
    command.push_back("OUTP" + std::to_string(channel) + " ON\n");

    wrongParam = "";
    if (!checkOffset(offset)) {
        wrongParam += signalDCFields.at(1);
        return false;
    }
    return true;
}

bool keysightCommand::catalogARB(std::string path, std::string & command)
{
    command = "MMEM:CAT:DATA:ARBitrary? \"" + path + "\"\n";

    return true;
}

bool keysightCommand::volatileCatalog(int channel, std::list<std::string> &command)
{
    command.push_back("SOUR" + std::to_string(channel) + ":DATA:VOL:CAT?\n");

    return true;
}

bool keysightCommand::loadARB(int channel, std::string file, std::list<std::string> &command)
{
    command.push_back("MMEM:LOAD:DATA" + std::to_string(channel) + " \"INT:\\BUILTIN\\" + file + "\"\n");

    return true;
}

bool keysightCommand::checkFreq(double freq, std::string signal, double ampl)
{
    if(signal == "SIN")
    {
        if(freq >= 1.0E-06 && ((ampl <=4 && freq <= 1.2E+08) || (ampl <=8 && freq <= 8.0E+07) || freq <= 6.0E+07))
            return true;
    }
    else if(signal == "NOIS")
    {
        if(freq >= 1.0E-03 && ((ampl <=4 && freq <= 1.2E+08) || (ampl <=8 && freq <= 8.0E+07) || freq <= 6.0E+07))
            return true;
    }
    else if(signal == "SQU" || signal == "PULS")
    {
        if(freq >= 1.0E-06 && ((ampl <=4 && freq <= 1.0E+08) || (ampl <=10 && freq <= 5.0E+07)))
            return true;
    }
    else if(signal == "RAMP" || signal == "TRI")
    {
        if(freq >= 1.0E-06 && freq <= 8.0E+05)
            return true;
    }
    else if(signal == "PRBS")
    {
        if(freq >= 1.0E-03 && ((ampl <=4 && freq <= 2.0E+08) || (ampl <=10 && freq <= 1.0E+08)))
            return true;
    }
    return false;
}

bool keysightCommand::checkVolt(double voltH, double voltL)
{
    if(voltH - voltL > 0.001 && voltH >= -4.999000000000001 && voltH <=5 && voltL >= -5 && voltL <= 4.999000000000001)
        return true;
    return false;
}

bool keysightCommand::checkPhase(double phase)
{
    if(phase >= -360 && phase <= 360)
        return true;
    return false;
}

bool keysightCommand::checkDcycle(double dcyc)
{
    if(dcyc >= 0.01 && dcyc <= 99.99)
        return true;
    return false;
}

bool keysightCommand::checkSymmetry(double symmetry)
{
    if(symmetry >= 0 && symmetry <= 100)
        return true;
    return false;
}

bool keysightCommand::checkWidth(double width, double ampl, double period)
{
    double minWidth;
    if(ampl<=4)
        minWidth = 5E-09;
    else
        minWidth = 8E-09;

    if(width >= minWidth && width <= period - minWidth)
        return true;
    return false;
}

bool keysightCommand::checkFrontInRange(double front, double ampl)
{
    double minValue;
    if(ampl<=4)
        minValue = 2.9E-09;
    else
        minValue = 3.3E-09;
    if(front >= minValue && front <= 1E-06)
        return true;
    return false;
}

bool keysightCommand::checkFrontConflict(double lead, double trail, double width, double period)
{
    if(width <= (period - (lead+trail)*0.625) && width >= ((lead+trail)*0.625))
        return true;
    return false;
}

bool keysightCommand::checkBand(double band, double ampl)
{
    if(band >= 1.0E-03 && ((ampl <=4 && band <= 1.2E+08) || (ampl <=8 && band <= 8.0E+07) || band <= 6.0E+07))
        return true;
    else
        return false;
}

bool keysightCommand::checkOffset(double offset)
{
    if(offset >= -5 && offset <= 5)
        return true;
    else
        return false;
}

bool keysightCommand::checkBrate(double brate, double ampl)
{
    if(brate >= 1.0E-03 && ((ampl <=4 && brate <= 2.0E+08) || (ampl <=10 && brate <= 1.0E+08)))
        return true;
    return false;
}

SwitchDriverCmd::SwitchDriverCmd()
{

}

bool SwitchDriverCmd::openSwitch(int bankNum, int channelNum, std::string & command)
{

    std::string cN = std::to_string(channelNum);
    if(channelNum < 10) cN = "0" + cN;
    command = "route:open (@" + std::to_string(bankNum) + cN + ")\n";

    if(checkBank(bankNum) && checkChannel(channelNum))
        return true;
    else return false;
}

bool SwitchDriverCmd::closeSwitch(int bankNum, int channelNum, std::string &command)
{
    std::string cN = std::to_string(channelNum);
    if(channelNum < 10) cN = "0" + cN;
    command = "route:close (@" + std::to_string(bankNum) + cN + ")\n";

    if(checkBank(bankNum) && checkChannel(channelNum))
        return true;
    else return false;
}

bool SwitchDriverCmd::setVoltage(int bankNum, std::string volt, std::string &command)
{
    command = "configure:bank" + std::to_string(bankNum) + " " + volt + "\n";

    if(checkBank(bankNum))
        return true;
    else return false;
}

bool SwitchDriverCmd::checkBank(int bankNum)
{
    if(bankNum == 1 || bankNum == 2)
        return true;
    else
        return false;
}

bool SwitchDriverCmd::checkChannel(int channelNum)
{
    if(channelNum >= 0 && channelNum <= 9)
        return true;
    else
        return false;
}
