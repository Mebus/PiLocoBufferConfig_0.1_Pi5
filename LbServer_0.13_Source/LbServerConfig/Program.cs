//-------------------------------------------------------------------
// File name   Program.cs
//             $Id: Program.cs 784 2013-07-24 06:34:53Z pischky $
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
// Description Main Program of LbServerConfig.
//-------------------------------------------------------------------

using System;
using System.Windows.Forms;

namespace net.sf.loconetovertcp.lbsrvcfg
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static int Main(String[] args)
        {
            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Model model = new Model(args);
                Application.Run(new LbServerConfigForm(model));
                return 0;
            }
            catch (Exception ex)
            {
                MessageBox.Show(null,
                    ""+ex.GetType().FullName+":\r\n"
                    + ex.Message + "\r\n"
                    + ex.StackTrace + "\r\n",
                    "LbServerConfig", 
                    MessageBoxButtons.OK, 
                    MessageBoxIcon.Error, 
                    MessageBoxDefaultButton.Button1);
                throw ex;
            }
        }
    }
}
