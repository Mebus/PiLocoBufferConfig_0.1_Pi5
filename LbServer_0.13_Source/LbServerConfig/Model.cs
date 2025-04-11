//-------------------------------------------------------------------
// File name   Model.cs
//             $Id: Model.cs 918 2017-07-02 12:46:33Z pischky $
//-------------------------------------------------------------------
// Author      Martin Pischky (mailto:martin@pischky.de)
//-------------------------------------------------------------------
// License:    This piece of code is part of the LoconetOverTcp 
//             project (http://loconetovertcp.sf.net) and therefore 
//             published under the terms of the GNU General Public 
//             Licence (http://www.gnu.org/licenses/gpl.html).
//             LocoNet is owned by Digitrax (http://www.digitrax.com)
//             Commercial use is subject to licensing by Digitrax!
//-------------------------------------------------------------------
// Description Class Model encapsulates the settings stored in 
//             lbserver-settings.bat. Writing and Reading is 
//             implemented here.
//-------------------------------------------------------------------

using System;
using System.IO;
using System.Management;
using System.Text;

namespace net.sf.loconetovertcp.lbsrvcfg
{

    /// <summary>
    /// The Model of LbServerConfig
    /// </summary>
    public class Model
    {

        /// <summary>
        /// Files used by read and write.
        /// </summary>
        public readonly String USER_FILE_NAME
            = System.Environment.ExpandEnvironmentVariables(
                                        @"%APPDATA%\LoconetOverTcp.sf.net\"
                                        + @"LbServer\lbserver-settings.bat");
        public const String LOCAL_FILE_NAME 
            = "lbserver-settings.bat";

        private bool useUserSettings = false; // set by commandline "/U"

        #region constructors

        public Model(String[] args)
        {
            this.parseArgs(args);
            this.ComPort = "COM1";
            this.TcpPort = 1234;
            this.ComSpeed = COM_SPEEDS[0];
            this.FlowCntrl = true;
        }

        #endregion

        #region properties

        /// <summary>
        /// The Com Port to use. Valid values depend on current Hardware.
        /// Use getComPorts() to get current valid values.
        /// </summary>
        public String ComPort
        {
            set;
            get;
        }

        /// <summary>
        /// The TCP Port to use. Accepted values are 0 .. 65535. 
        /// </summary>
        public UInt16 TcpPort
        {
            set;
            get;
        }

        /// <summary>
        /// The baudrate to use. See COM_SPEEDS for valid values.
        /// </summary>
        public UInt32 ComSpeed
        {
            set;
            get;
        }

        /// <summary>
        /// Use CTS Flow Control.
        /// </summary>
        public Boolean FlowCntrl
        {
            set;
            get;
        }

        #endregion

        #region public methods

        /// <summary>
        /// Update my properties from file.
        /// </summary>
        public void read()
        {
            String fileName = LOCAL_FILE_NAME;
            if (useUserSettings && File.Exists(USER_FILE_NAME))
            {
                fileName = USER_FILE_NAME;
            }
            using (TextReader rdr = new StreamReader(fileName, Encoding.Default))
            {
                String line;
                while ((line = rdr.ReadLine()) != null)
                {
                    for (int i = 0; i < FILE_TAGS.Length; i++)
                    {
                        if (line.ToUpperInvariant().StartsWith(FILE_TAGS[i]))
                        {
                            String val = line.Substring(FILE_TAGS[i].Length).Trim();
                            this.setValOnRead(i, val);
                            break;
                        }
                    }
                }
                rdr.Close();
            }
        }

        /// <summary>
        /// Write my properties to file. Read old file as template.
        /// </summary>
        public void write()
        {
            String fileName = LOCAL_FILE_NAME;
            String oldName = LOCAL_FILE_NAME;
            if (useUserSettings)
            {
                fileName = USER_FILE_NAME;
                if (File.Exists(USER_FILE_NAME)) oldName = USER_FILE_NAME;
            }
            FileInfo tmpFileInfo = new FileInfo(fileName + ".tmp");
            string bakFileName = fileName + ".bak";
            if (!tmpFileInfo.Directory.Exists) tmpFileInfo.Directory.Create();
            using (FileStream tmpStream = tmpFileInfo.Open(FileMode.Create))
            using (TextWriter tmpWriter = new StreamWriter(tmpStream, Encoding.Default))
            using (TextReader rdr = new StreamReader(oldName, Encoding.Default))
            {
                String line;
                while ((line = rdr.ReadLine()) != null)
                {
                    bool written = false;
                    for (int i = 0; i < FILE_TAGS.Length; i++)
                    {
                        if (line.ToUpperInvariant().StartsWith(FILE_TAGS[i]))
                        {
                            tmpWriter.Write(line.Substring(0, FILE_TAGS[i].Length));
                            tmpWriter.WriteLine(this.getValOnWrite(i));
                            written = true;
                            break;
                        }
                    }
                    if (!written) tmpWriter.WriteLine(line);
                }
                rdr.Close();
                tmpWriter.Close();
                tmpStream.Close();
                if (File.Exists(bakFileName)) File.Delete(bakFileName);
                if (File.Exists(fileName)) File.Move(fileName, bakFileName);
                tmpFileInfo.MoveTo(fileName);
            }
        }

        #endregion

        #region public static methodes and fields

        public static String[] getComPorts()
        {
            String[] ports;
            ManagementObjectSearcher mos = new ManagementObjectSearcher(
                                "root\\WMI", "SELECT * FROM MSSerial_PortName");
            ManagementObjectCollection moc = mos.Get();
            ports = new String[ moc.Count ];
            int i = 0;
            foreach (ManagementObject queryObj in moc)
            {
                ports[i] = queryObj["PortName"].ToString();
                //queryObj.queryObj["Active"]);
                //queryObj["InstanceName"]);
                i++;
            }
            return ports;
        }
        
        /*  other usefull query:
            try
            {
                ManagementObjectSearcher searcher = 
                    new ManagementObjectSearcher("root\\CIMV2", 
                    "SELECT * FROM Win32_PnPEntity"); 

                foreach (ManagementObject queryObj in searcher.Get())
                {
                    if( queryObj["Caption"].ToString().IndexOf("COM")<0 ) continue; //need really check for "COM[0-9]*" substring
                    Console.WriteLine("-----------------------------------");
                    Console.WriteLine("Win32_PnPEntity instance");
                    Console.WriteLine("-----------------------------------");
                    Console.WriteLine("Caption: {0}", queryObj["Caption"]);
                    Console.WriteLine("ClassGuid: {0}", queryObj["ClassGuid"]);
                    Console.WriteLine("Description: {0}", queryObj["Description"]);
                    Console.WriteLine("DeviceID: {0}", queryObj["DeviceID"]);
                    Console.WriteLine("Manufacturer: {0}", queryObj["Manufacturer"]);
                    Console.WriteLine("Name: {0}", queryObj["Name"]);
                    Console.WriteLine("PNPDeviceID: {0}", queryObj["PNPDeviceID"]);
                    Console.WriteLine("Service: {0}", queryObj["Service"]);
                }
            }
            catch (ManagementException e)
            {
                MessageBox.Show("An error occurred while querying for WMI data: " + e.Message);
            }
        */

        public static UInt32[] COM_SPEEDS = { 19200, 38400, 57600, 115200 };

        #endregion

        #region private methodes

        private const String COM_PORT_PREFIX = @"\\.\";

        private readonly String[] FILE_TAGS = { "SET COM_PORT=", 
                                                "SET TCP_PORT=", 
                                                "SET COM_SPEED=", 
                                                "SET FLOW_CNTL="  };

        private void setValOnRead(int i, String val)
        {
            switch (i)
            {
                case 0:
                    // we assume "1", "COM1" or "\\.\COM1"
                    try
                    {
                        val = "COM" + Int16.Parse(val);
                    }
                    catch (OverflowException)
                    {
                    }
                    catch (FormatException)
                    {
                    }
                    if (val.StartsWith(COM_PORT_PREFIX))
                    {
                        val = val.Substring(COM_PORT_PREFIX.Length);
                    }
                    this.ComPort = val;
                    break;
                case 1:
                    this.TcpPort = Convert.ToUInt16(val);
                    break;
                case 2:
                    this.ComSpeed = Convert.ToUInt32(val);
                    break;
                case 3:
                    this.FlowCntrl = Convert.ToBoolean(val);
                    break;
            }
        }

        private String getValOnWrite(int i)
        {
            switch (i)
            {
                case 0:
                    return COM_PORT_PREFIX + this.ComPort;
                case 1:
                    return this.TcpPort.ToString();
                case 2:
                    return this.ComSpeed.ToString();
                case 3:
                    return this.FlowCntrl.ToString().ToLowerInvariant();
            }
            return "";
        }

        private void parseArgs(String[] args)
        {
            foreach (String arg in args)
            {
                String p = arg.ToLower();
                if (p == "/u" || p == "-u") useUserSettings = true;
            }
        }

        #endregion

        #region ToString(), Equals(object), GetHashCode()

        public override string ToString()
        {
            return "Model(ComPort=" + this.ComPort 
                 + ", TcpPort=" + this.TcpPort
                 + ", ComSpeed=" + this.ComSpeed
                 + ", FlowCntrl=" + this.FlowCntrl + ")";
        }

        public override bool Equals(object obj)
        {
            return base.Equals(obj);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        #endregion

    }
}
