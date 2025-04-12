//-------------------------------------------------------------------
// File name   LbServerConfigForm.cs
//             $Id: LbServerConfigForm.cs 1328 2023-05-07 09:50:57Z pischky $
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
// Description Class LbServerConfigForm implements the dialog to edit
//             the propierties. The settings are read on start. User 
//             can modify the properties and than press "Save".
//-------------------------------------------------------------------

using System;
using System.Drawing;
using System.Windows.Forms;
using ManagementException = System.Management.ManagementException;
using TextFormat          = System.Management.TextFormat;

namespace net.sf.loconetovertcp.lbsrvcfg
{

    /// <summary>
    /// Form of LbServerConfig
    /// </summary>
    public partial class LbServerConfigForm : Form
    {

        Model myModel;

        public LbServerConfigForm(Model model)
        {
            myModel = model;
            InitializeComponent();
            foreach (UInt32 s in Model.COM_SPEEDS)
            {
                comboBoxComSpeed.Items.Add(s);
            }
            comboBoxFlowControl.Items.Add(true);
            comboBoxFlowControl.Items.Add(false);
        }

        #region event handling

        private void LbServerConfigForm_Load(object sender, EventArgs e)
        {
            this.updateComPortsToComboBox();
            this.loadFromFile();
        }

        private void buttonUpdatePorts_Click(object sender, EventArgs e)
        {
            Object s = comboBoxComPort.SelectedItem;
            this.updateComPortsToComboBox();
            comboBoxComPort.SelectedItem = s;
        }

        private void textBoxTcpPort_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!Char.IsDigit(e.KeyChar))
            {
                e.Handled = !Char.IsControl(e.KeyChar);
            }
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            if (this.writeToFile())
            {
                this.Close();
            }
        }

        private void buttonDiscard_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        #endregion

        #region private helper methodes

        private void loadFromFile()
        {
            DialogResult res = DialogResult.None;
            do
            {
                try
                {
                    myModel.read();
                    res = DialogResult.OK; //avoid endless loop after one fail
                }
                catch (Exception ex)
                {
                    res = MessageBox.Show(this, "Read failed\n\r" + ex.Message,
                        this.Text + ": Error", MessageBoxButtons.RetryCancel,
                        MessageBoxIcon.Error, MessageBoxDefaultButton.Button2);
                }
            } while (res == DialogResult.Retry);
            this.updateFieldsFromModel();
        }

        private bool writeToFile()
        {
            DialogResult res = DialogResult.None;
            do
            {
                try
                {
                    this.updateModelFromFields();
                    myModel.write();
                    res = DialogResult.OK;
                }
                catch (Exception ex)
                {
                    res = MessageBox.Show(this, "Write failed\n\r" + ex.Message,
                        this.Text + ": Error", MessageBoxButtons.RetryCancel,
                        MessageBoxIcon.Error, MessageBoxDefaultButton.Button2);
                }
            } while (res == DialogResult.Retry);
            return res == DialogResult.OK;
        }

        private void updateFieldsFromModel()
        {
            comboBoxComPort.SelectedItem = myModel.ComPort;
            textBoxTcpPort.Text = myModel.TcpPort.ToString();
            comboBoxComSpeed.SelectedItem = myModel.ComSpeed;
            comboBoxFlowControl.SelectedItem = myModel.FlowCntrl;
        }

        private void updateModelFromFields()
        {
            String p = (String)comboBoxComPort.SelectedItem;
            if (p == null || p.Trim() == "")
            {
                throw new ApplicationException("invalid value for COM_PORT.");
            }
            myModel.ComPort = p;
            myModel.TcpPort = UInt16.Parse(textBoxTcpPort.Text);
            myModel.ComSpeed = UInt32.Parse(comboBoxComSpeed.SelectedItem.ToString());
            myModel.FlowCntrl = Boolean.Parse(comboBoxFlowControl.SelectedItem.ToString());
        }

        private void updateComPortsToComboBox()
        {
            comboBoxComPort.Items.Clear();
            String[] ports = new String[0];
            try
            {
                ports = Model.getComPorts();
            }
            catch (ManagementException e)
            {
                MessageBox.Show(
                    this,
                    "An error occurred while querying for available "
                    + "COM ports. May be you will have to plugin "
                    + "hardware or install drivers."
                    + "\r\n"
                    + "\r\n" + e.GetType().FullName
                    + "\r\nMessage: " + e.Message
                    + "\r\nErrorCode: " + e.ErrorCode + " (0x" 
                                     + ((Int32)e.ErrorCode).ToString("X8") + ")"
                    + "\r\nErrorInformation: " + e.ErrorInformation.GetText(
                                                                TextFormat.Mof),
                    this.Text + ": Error",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            comboBoxComPort.Items.AddRange(ports);
        }

        #endregion

    }
}
