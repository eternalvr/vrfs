using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO.Ports;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using FSUIPC;

namespace VRFSConnect
{
    public partial class Form1 : Form
    {
        private const int NumPorts = 4;
        private readonly Offset<short> _adf1 = new Offset<short>(0x034c);
        private readonly Offset<short> _adfext = new Offset<short>(0x0356);
        private readonly Offset<short> _com1 = new Offset<short>(0x034e);
        private readonly Offset<short> _com1Standby = new Offset<short>(0x311a);
        private readonly Offset<short> _ecp1 = new Offset<short>(0x7394);
        private readonly Offset<short> _ecp2 = new Offset<short>(0x7395);
        private readonly Offset<short> _misccontrols = new Offset<short>(0x78ed);
        private readonly Offset<short> _nav1 = new Offset<short>(0x0350);
        private readonly Offset<short> _nav1Standby = new Offset<short>(0x311e);
        private readonly Offset<short> _transponderMode = new Offset<short>(0x7b91);
        private readonly Offset<short> _transpondervalue = new Offset<short>(0x0354);
        private readonly Offset<short> _squawkIdent = new Offset<short>(0x7b93);

        private readonly SolidBrush _reportsBackgroundBrushGreen = new SolidBrush(Color.Green);
        private readonly SolidBrush _reportsBackgroundBrushRed = new SolidBrush(Color.Red);
        private readonly SolidBrush _reportsBackgroundBrushYellow = new SolidBrush(Color.Yellow);
        private readonly SolidBrush _reportsForegroundBrush = new SolidBrush(Color.Black);
        private readonly SolidBrush _reportsForegroundBrushSelected = new SolidBrush(Color.White);
        private Dictionary<string, SerialPort> _availablePorts;
        private bool _fsuipcConnected;
/*
        private ManagementObjectCollection _manObjReturn;
*/
/*
        private ManagementObjectSearcher _manObjSearch;
*/
        private string[] _serialBuffer;
        private SerialPort[] _sp;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            FSUIPCConnection.Close();
            _fsuipcConnected = false;
            for (var i = 0; i < NumPorts; i++)
            {
                if (_sp[i] != null && _sp[i].IsOpen)
                {
                    _sp[i].Close();
                }
            }
            foreach (var port in _availablePorts)
            {
                if (port.Value != null && port.Value.IsOpen)
                {
                    try
                    {
                        port.Value.Close();
                    }
                    catch (Exception) { }
                    
                
                
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            UpdateFsuipc();

            var serialText = ConstructSerialString();
            textBox1.Text = serialText;
            if (_sp[(int) SerialPortName.Rmp] != null && _sp[(int) SerialPortName.Rmp].IsOpen)
            {
                try
                {
                    _sp[(int) SerialPortName.Rmp].WriteLine(serialText);
                }
                catch (Exception)
                {
                    // ignored
                }
            }

            SendEcpInfos();
        }

        private void SendEcpInfos()
        {
            try
            {
                var serialText = "%" + _ecp1.Value + "," + _ecp2.Value + "#";

                if (_sp[(int) SerialPortName.Ecp] != null && _sp[(int) SerialPortName.Ecp].IsOpen)
                {
                    _sp[(int) SerialPortName.Ecp].WriteLine(serialText);
                }
            }
            catch (Exception)
            {
                // ignored
            }
        }

        private string ConstructSerialString()
        {
            var s = "";
            var com1S = _com1.Value.ToString("X");
            s += "C1" + com1S.Substring(0, 2) + "." + com1S.Substring(2, 2) + "|";
            var com1Sb = _com1Standby.Value.ToString("X");
            s += "1" + com1Sb.Substring(0, 2) + "." + com1Sb.Substring(2) + ";";

            var nav1S = _nav1.Value.ToString("X");
            if (nav1S.Length == 3)
            {
                nav1S = "0" + nav1S;
            }
            s += "I1" + nav1S.Substring(0, 2) + "." + nav1S.Substring(2, 2) + "|";
            var nav1Sb = _nav1Standby.Value.ToString("X");
            s += "1" + nav1Sb.Substring(0, 2) + "." + nav1Sb.Substring(2, 2) + ";";

            var adfa = _adf1.Value.ToString("X");
            var adfb = _adfext.Value.ToString("X");

            var t = 0;
            var f = 0;

            if (adfb != "0")
            {
                if (adfb.Length == 3)
                {
                    adfb = "0" + adfb;
                }
                t = int.Parse(adfb.Substring(0, 2));
                f = int.Parse(adfb.Substring(2, 2));
            }


            var fr = int.Parse(adfa);
            s += "A" + (t + fr) + "." + f + ";";

            return s;
        }

        private void LoadPorts()
        {
            var rmpValue = cbPortsRMP.Text;
            var mcduValue = cbPortsMCDU.Text;
            var ecamValue = cbPortsECAM.Text;
            var tcasValue = cbPortsTcas.Text;

            var ports = SerialPort.GetPortNames();

            cbPortsRMP.Items.Clear();
            cbPortsMCDU.Items.Clear();
            cbPortsECAM.Items.Clear();
            cbPortsTcas.Items.Clear();

            foreach (var comport in ports)
            {
                if (!listCom.Items.Contains(comport))
                {
                    listCom.Items.Add(comport);
                    if (!_availablePorts.ContainsKey(comport))
                    {
                        _availablePorts.Add(comport, new SerialPort(comport));
                        _availablePorts[comport].Open();
                        _availablePorts[comport].DataReceived += DataReceived;
                    }
                }

                cbPortsRMP.Items.Add(comport);
                cbPortsMCDU.Items.Add(comport);
                cbPortsECAM.Items.Add(comport);
                cbPortsTcas.Items.Add(comport);
            }

            cbPortsECAM.Text = ecamValue;
            cbPortsMCDU.Text = mcduValue;
            cbPortsRMP.Text = rmpValue;
            cbPortsTcas.Text = tcasValue;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            _availablePorts = new Dictionary<string, SerialPort>();
            _sp = new SerialPort[NumPorts];
            _serialBuffer = new string[NumPorts];
            LoadPorts();
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

/*
        private string GetPnpDeviceIdByPort(string portName)
        {
            _manObjSearch = new ManagementObjectSearcher("Select * from Win32_SerialPort");
            _manObjReturn = _manObjSearch.Get();

            foreach (ManagementObject manObj in _manObjReturn)
            {
                if (manObj["DeviceID"].ToString() == portName)
                {
                    return manObj["PNPDeviceID"].ToString();
                }
            }
            return "UNKNOWN";
        }
*/

        private void ParseSerialData(int v)
        {
            var receivedData = _serialBuffer[v];
            AddText(receivedData);
            if (receivedData == "") return;

            #region RMP

            if (receivedData.StartsWith("$"))
            {
                receivedData = receivedData.Replace("$", "");
                if (receivedData.Length == 0) return;
                if (receivedData[receivedData.Length - 1] != ';')
                {
                    return;
                }

                var parts = receivedData.Split(';');
                for (var i = 0; i < parts.Length; i++)
                {
                    if (parts[i].Trim().Length == 0)
                    {
                        continue;
                    }
                    parts[i] = parts[i].Replace("-", "").Trim();

                    var pipePos = parts[i].IndexOf("|", StringComparison.Ordinal);


                    switch (parts[i][0])
                    {
                        case 'C':

                            if (pipePos == -1) continue;
                            var com1A = parts[i].Substring(1, pipePos - 1);
                            var com1S = parts[i].Substring(pipePos + 1);
                            var a = Convert.ToInt32(com1A.Substring(1, 2) + com1A.Substring(4, 2), 16);
                            var s = Convert.ToInt32(com1S.Substring(1, 2) + com1S.Substring(4, 2), 16);

                            _com1.Value = (short) a;
                            _com1Standby.Value = (short) s;
                            UpdateFsuipc();
                            break;
                        case 'I':

                            if (pipePos == -1) continue;


                            var nava = parts[i].Substring(1, pipePos - 1);
                            var navs = parts[i].Substring(pipePos + 1);
                            var na = Convert.ToInt32(nava.Substring(1, 2) + nava.Substring(4, 2), 16);
                            var ns = Convert.ToInt32(navs.Substring(1, 2) + navs.Substring(4, 2), 16);

                            _nav1.Value = (short) na;
                            _nav1Standby.Value = (short) ns;
                            UpdateFsuipc();
                            break;
                    }
                }
            }

            #endregion

            while (receivedData.Length != 0)
            {
                if (receivedData.StartsWith("!")) // mcdu
                {
                    #region MCDU

                    receivedData = receivedData.Substring(1);
                    if (receivedData.Length == 0)
                    {
                        return;
                    }
                    var r = receivedData[0];
                    if (r != 0)
                    {
                        var b = new Offset<short>(0x78EC, true);
                        if (r >= 49 && r <= 63)
                        {
                            b.Value = (short) (r - 49 + 28);
                        }
                        if (r >= 64 && r <= 90)
                        {
                            b.Value = (short) (r - 64);
                        }

                        if (r >= 18 && r < 30) // LSK1-6
                        {
                            b.Value = (short) (r + 20);
                        }


                        switch ((int) r)
                        {
                            case 17:
                                b.Value = 50; //CLR
                                break;
                            case 48:
                                b.Value = 27; // 0
                                break;
                            case 16:
                                b.Value = 51; //OVFY
                                break;
                            case 1:
                                b.Value = 56; //DIR
                                break;
                            case 2:
                                b.Value = 57; //PROG
                                break;
                            case 3:
                                b.Value = 58; //PERF
                                break;
                            case 4:
                                b.Value = 59; //INIT
                                break;
                            case 5:
                                b.Value = 60; //DATA
                                break;
                            case 6:
                                b.Value = 61; //FPLN
                                break;
                            case 7:
                                b.Value = 62; //RADNAV
                                break;
                            case 8:
                                b.Value = 63; //FUELPRED
                                break;
                            case 9:
                                b.Value = 64; //SECFLPN
                                break;
                            case 31:
                                b.Value = 65; //MCDUMENU
                                break;
                            case 12:
                                b.Value = 66; //AIRPORT
                                break;
                            case 30:
                                b.Value = 69; //UP
                                break;
                            case 14:
                                b.Value = 70; //DOWN
                                break;
                            case 15:
                                b.Value = 68; //RIGHT/NEXTPAGE
                                break;
                            case 46:
                                b.Value = 55; // POINT
                                break;
                            case 47:
                                b.Value = 53; // SLASH
                                break;
                            case 43:
                                b.Value = 54; // PLUS
                                break;
                            case 45:
                                b.Value = 52; // SPACE
                                break;
                        }
                        AddText(b.Value.ToString());
                        UpdateFsuipc();
                    }

                    #endregion
                }
                else if (receivedData.StartsWith("%"))
                {
                    #region ECP

                    receivedData = receivedData.Substring(1);
                    var param = short.Parse(receivedData);
                    if (param != 0)
                    {
                        _misccontrols.Value = param;
                        UpdateFsuipc();
                        AddText(param.ToString());
                    }

                    #endregion
                }
                else if (receivedData.StartsWith("["))
                {
                    #region TCAS

                    receivedData = receivedData.Substring(1);
                    char t = receivedData[0];
                    char u = receivedData[1];
                    char vx = receivedData[2];
                    string std = Convert.ToString(t, 2).PadLeft(8, '0');
                    
                    short tv = 0,av = 0;

                    tv = ((t & 1 << 5) != 0) ? (short) 1 : tv;
                    av = ((vx & 1 << 0) != 0) ? (short) 1 : av;
                    if (_transponderMode.Value == 1 && tv == 1)
                    {
                        _transponderMode.Value = 0;
                        UpdateFsuipc();
                    }
                    if (_transponderMode.Value == 0 && av == 1)
                    {
                        _transponderMode.Value = 1;
                        UpdateFsuipc();
                    }
                        
                    _misccontrols.Value = ((u & 1 << 5) != 0) ? (short)36 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString());}
                    _misccontrols.Value = ((u & 1 << 6) != 0) ? (short)37 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString()); }
                    _misccontrols.Value = ((u & 1 << 3) != 0) ? (short)38 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString()); }
                    _misccontrols.Value = ((u & 1 << 4) != 0) ? (short)39 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString()); }
                    _misccontrols.Value = ((u & 1 << 2) != 0) ? (short)40 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString()); }
                    _misccontrols.Value = ((u & 1 << 0) != 0) ? (short)41 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString()); }
                    _misccontrols.Value = ((u & 1 << 1) != 0) ? (short)42 : _misccontrols.Value; if (_misccontrols.Value != 0) { UpdateFsuipc(); AddText(_misccontrols.Value.ToString()); }
                    _squawkIdent.Value =  ((t & 1 << 0) != 0) ? (short)1 : (short)0;
                    



                    receivedData = receivedData.Substring(3);
                    
                    byte[] transponderValue = ToBcd(int.Parse(receivedData));
                    _transpondervalue.Value = (short) (transponderValue[0] | (transponderValue[1] << 8));
                    
                    UpdateFsuipc();
                    #endregion
                }
                else
                {
                    receivedData = receivedData.Substring(1);
                }
            }

            _serialBuffer[v] = "";
        }

        public void AddText(string text)
        {
            Invoke((MethodInvoker) delegate
            {


                txtSerialOut.Text += text;
                txtSerialOut.SelectionStart = txtSerialOut.TextLength;
                txtSerialOut.ScrollToCaret();
            }
                );
        }

        public static byte[] ToBcd(int value)
        {
            if (value < 0 || value > 99999999)
                throw new ArgumentOutOfRangeException(nameof(value));
            var ret = new byte[2];
            for (var i = 0; i < 2; i++)
            {
                ret[i] = (byte) (value%10);
                value /= 10;
                ret[i] |= (byte) ((value%10) << 4);
                value /= 10;
            }
            return ret;
        }

        private void SerialConnect(int v, string port)
        {
            if (_sp[v] != null && _sp[v].IsOpen)
            {
                _sp[v].Close();
            }
            _sp[v] = null;
            _sp[v] = new SerialPort(port);
            _sp[v].Open();
            _sp[v].DataReceived += DataReceived;
            _serialBuffer[v] = "";
        }

        private void DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            var r = (SerialPort) sender;
            var iPort = 0;

            for (var i = 0; i < NumPorts; i++)
            {
                if (r.Equals(_sp[i]))
                {
                    iPort = i;
                    break;
                }
            }

            var s = r.ReadExisting();


            // find identification




            // find message end
            var bufferEnd = s.IndexOf("#", StringComparison.Ordinal);
            if (bufferEnd == -1)
            {

                _serialBuffer[iPort] += s;

                var idt = Regex.Match(_serialBuffer[iPort], "_!([a-zA-Z]+)!_");
                if (idt.Success)
                {
                    AssignDevice(idt.Groups[1].Value, r);
                    _serialBuffer[iPort] = _serialBuffer[iPort].Replace("_!" + idt.Groups[1].Value + "!_","");
                }
            }
            while((bufferEnd = s.IndexOf("#", StringComparison.Ordinal))!=-1)
            {
                _serialBuffer[iPort] += s.Substring(0, bufferEnd);
                ParseSerialData(iPort);
                //_serialBuffer[iPort] = s.Substring(bufferEnd + 1);
                s = s.Substring(bufferEnd + 1);
            }
        }

        private void AssignDevice(string idString, SerialPort r)
        {
            switch (idString.ToLower())
            {
                case "ecp":
                    if (_sp[(int)SerialPortName.Ecp] == null)
                    {
                        _sp[(int)SerialPortName.Ecp] = r;
                    }

                    break;
                case "mcdu":
                    if (_sp[(int)SerialPortName.Mcdu] == null)
                    {
                        _sp[(int)SerialPortName.Mcdu] = r;
                    }

                    break;
                case "rmp":
                    if (_sp[(int)SerialPortName.Rmp] == null)
                    {
                        _sp[(int)SerialPortName.Rmp] = r;
                    }

                    break;
                case "tcas":
                    if (_sp[(int)SerialPortName.Tcas] == null)
                    {
                        _sp[(int)SerialPortName.Tcas] = r;
                    }
                    break;

            }
        }
        private void FSUIPCTimer_Tick(object sender, EventArgs e)
        {
            var fs = FSUIPCConnection.FlightSimVersionConnected;

            if (cbPortsRMP.Text != "")
            {
                try
                {
                    if (_sp[(int) SerialPortName.Rmp] != null)
                    {
                        if (!_sp[(int) SerialPortName.Rmp].IsOpen ||
                            _sp[(int) SerialPortName.Rmp].PortName != cbPortsRMP.Text)
                        {
                            SerialConnect((int) SerialPortName.Rmp, cbPortsRMP.Text);
                        }
                    }
                    else
                    {
                        SerialConnect((int) SerialPortName.Rmp, cbPortsRMP.Text);
                    }
                }
                catch (Exception)
                {
                    // ignored
                }
                //Properties.Settings.Default.RMP = 
            }
            RMPStatusLabel.Text = (_sp[(int) SerialPortName.Rmp] != null && _sp[(int) SerialPortName.Rmp].IsOpen)
                ? "RMP " + _sp[(int) SerialPortName.Rmp].PortName + " OK"
                : "RMP Not Connected";

            if (cbPortsMCDU.Text != "")
            {
                try
                {
                    if (_sp[(int) SerialPortName.Mcdu] != null)
                    {
                        if (!_sp[(int) SerialPortName.Mcdu].IsOpen ||
                            _sp[(int) SerialPortName.Mcdu].PortName != cbPortsMCDU.Text)
                        {
                            SerialConnect((int) SerialPortName.Mcdu, cbPortsMCDU.Text);
                        }
                    }
                    else
                    {
                        SerialConnect((int) SerialPortName.Mcdu, cbPortsMCDU.Text);
                    }
                }
                catch (Exception)
                {
                    // ignored
                }
            }
            MCDUStatusLabel.Text = (_sp[(int) SerialPortName.Mcdu] != null && _sp[(int) SerialPortName.Mcdu].IsOpen)
                ? "MCDU " + _sp[(int) SerialPortName.Mcdu].PortName + " OK"
                : "MCDU Not Connected";

            if (cbPortsECAM.Text != "")
            {
                try
                {
                    if (_sp[(int) SerialPortName.Ecp] != null)
                    {
                        if (!_sp[(int) SerialPortName.Ecp].IsOpen ||
                            _sp[(int) SerialPortName.Ecp].PortName != cbPortsECAM.Text)
                        {
                            SerialConnect((int) SerialPortName.Ecp, cbPortsECAM.Text);
                        }
                    }
                    else
                    {
                        SerialConnect((int) SerialPortName.Ecp, cbPortsECAM.Text);
                    }
                }
                catch (Exception)
                {
                    // ignored
                }
            }
            ECAMStatusLabel.Text = (_sp[(int) SerialPortName.Ecp] != null && _sp[(int) SerialPortName.Ecp].IsOpen)
                ? "ECAM " + _sp[(int) SerialPortName.Ecp].PortName + " OK"
                : "ECAM Not Connected";


            if (cbPortsTcas.Text != "")
            {
                try
                {
                    if (_sp[(int)SerialPortName.Tcas] != null)
                    {
                        if (!_sp[(int)SerialPortName.Tcas].IsOpen ||
                            _sp[(int)SerialPortName.Tcas].PortName != cbPortsTcas.Text)
                        {
                            SerialConnect((int)SerialPortName.Tcas, cbPortsTcas.Text);
                        }
                    }
                    else
                    {
                        SerialConnect((int)SerialPortName.Tcas, cbPortsTcas.Text);
                    }
                }
                catch (Exception)
                {
                    // ignored
                }
                //Properties.Settings.Default.RMP = 
            }
            if (_sp[(int) SerialPortName.Tcas] != null && _sp[(int) SerialPortName.Tcas].IsOpen)
            {
                TcasStatusLabel.Text = "TCAS " + _sp[(int) SerialPortName.Tcas].PortName + " OK";
                TcasStatusLabel.BackColor = Color.ForestGreen;
            }
            else
            {
                TcasStatusLabel.Text = "TCAS Not Connected";
                TcasStatusLabel.BackColor = Color.Yellow;
            }
                 


            if (fs.ToString() != "Any")
            {
                return;
            }
            try
            {
                FSUIPCConnection.Open();
                timer1.Enabled = true;
                _fsuipcConnected = true;
                statusFSUIPC.Text = @"FSUIPC OK";
            }
            catch (Exception)
            {
                timer1.Enabled = false;
                statusFSUIPC.Text = @"FSUIPC Not Connected";
                _fsuipcConnected = false;
            }
        }

        private void UpdateFsuipc()
        {
            if (_fsuipcConnected)
            {
                FSUIPCConnection.Process();
            }
        }

        private void btnReload_Click(object sender, EventArgs e)
        {
            LoadPorts();
        }

        private void listCom_DrawItem(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();
            var selected = ((e.State & DrawItemState.Selected) == DrawItemState.Selected);

            var index = e.Index;
            if (index >= 0 && index < listCom.Items.Count)
            {
                var text = listCom.Items[index].ToString();
                var g = e.Graphics;

                //background:
                var backgroundBrush = _reportsBackgroundBrushRed;
                if (_availablePorts[text] != null)
                {
                    backgroundBrush = (_availablePorts[text].IsOpen)
                        ? _reportsBackgroundBrushGreen
                        : _reportsBackgroundBrushYellow;
                }
                g.FillRectangle(backgroundBrush, e.Bounds);

                //text:
                var foregroundBrush = (selected) ? _reportsForegroundBrushSelected : _reportsForegroundBrush;
                g.DrawString(text, e.Font, foregroundBrush, listCom.GetItemRectangle(index).Location);
            }

            e.DrawFocusRectangle();
        }

        private enum SerialPortName
        {
            Rmp,
            Mcdu,
            Ecp,
            Tcas
        };

        
}
}