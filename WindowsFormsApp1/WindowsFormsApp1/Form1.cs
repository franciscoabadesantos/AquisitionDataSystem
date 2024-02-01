using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using Newtonsoft.Json;  //adicionado
using Newtonsoft.Json.Linq; //adicionado
using System.Diagnostics;
using System.Threading;
// adicionado para por no servidor
using System.Net.Http;
using System.Xml;
using System.Text.RegularExpressions;

// adicionado para criar um ficheiro
using System.IO;



namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        //output
        private int digital0 = 0;
        private int digital1 = 0;
        //input
        private int analog2 = 1;
        private int analog3 = 1;
        private int analog4 = 1;
        private int analog5 = 1;
        private int digital6 = 1;
        private int digital7 = 1;
        private int Dbidirecional = 0;
        private int Dvirtual = 0;
        //other values
        private string frequency;
        private string samples;
        private string printText = "";


        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            serialPort1.BaudRate = 9600; // Set the baud rate to match your device configuration
            serialPort1.Open();

            // Register the event handler
            serialPort1.DataReceived += serialPort1_DataReceived;
        }


        private void Configuration_input(object sender, EventArgs e)
        {
            CheckBox clickedCheckbox = (CheckBox)sender;
            int i;
            if (clickedCheckbox.Checked)    
            {   i = 1;  
            } else   {   i = 0;  }

            switch (clickedCheckbox.Name)
            {
                case "checkBox1":
                    analog2 = i;
                    break;
                case "checkBox2":
                    analog3 = i;
                    break;
                case "checkBox3":
                    analog4 = i;
                    break;
                case "checkBox4":
                    analog5 = i;
                    break;
                case "checkBox5":
                    digital6 = i;
                    break;
                case "checkBox6":
                    digital7 = i;
                    break;
                case "checkBox7":
                    Dbidirecional = i;
                    break;
                case "checkBox8":
                    Dvirtual = i;
                    break;
            }


            string send = "{\"A2\":" + analog2 + ", \"A3\":" + analog3 + ", \"A4\":" + analog4 + ", \"A5\":" + analog5
                + ", \"D6\":" + digital6 + ", \"D7\":" + digital7 + ", \"DB\":" + Dbidirecional + ", \"DV\":" + Dvirtual + "}"+'\n';

            richTextBox1.Text = send;
            serialPort1_DataSender(send);

        }

        private void checkBox7_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox clickedCheckbox = (CheckBox)sender;
            if (clickedCheckbox.Checked)
            {
                digital0 = 1;
            }
            else 
            { 
                digital0 = 0; 
            }
            string send = "{\"D0\":" + digital0 + "}" + '\n';
            richTextBox1.Text = send;
            serialPort1_DataSender(send);

        }

        private void radioButtonV_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton selectedRadioButton = (RadioButton)sender;

            if (selectedRadioButton.Checked)
            {
                Dvirtual = 0;
            }
            else { Dvirtual = 1; }

            string send = "{\"v\":" + Dvirtual + "}" + '\n';
            richTextBox1.Text = send;
            serialPort1_DataSender(send);

        }

        private void radioButtonB_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton selectedRadioButton = (RadioButton)sender;

            if (selectedRadioButton.Checked)
            {
                 Dbidirecional= 1;
            }
            else { Dbidirecional = 0; }

            string send = "{\"b\":" + Dbidirecional + "}" + '\n';
            richTextBox1.Text = send;
            serialPort1_DataSender(send);
        }


        private void button1_Click(object sender, EventArgs e)
        {
            frequency = textBox3.Text;
            string send = "{\"f\":" + frequency + "}" + '\n';
            richTextBox1.Text = send;
            serialPort1_DataSender(send);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            samples = textBox4.Text;
            string send = "{\"n\":" + samples + "}" + '\n';
            richTextBox1.Text = send;
            serialPort1_DataSender(send);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            richTextBox1.Text = "";
            if (radioButton5.Checked)
            {
                printText = "";
            }
            if (radioButton6.Checked)
            {

                var filePath = @"C:\Users\Tiago Zorro\Desktop\Faculdade\4ºAno\2ºSemestre\SAD\Trabalho3\file.json";
                if (File.Exists(filePath)) // Check if file exists
                {
                    // Delete the file
                    File.Delete(filePath);
                }

            }
        }

        private void radioButton6_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton6.Checked)
            {
                button3.Text = "Delete File";

                string fileName = @"C:\Users\Tiago Zorro\Desktop\Faculdade\4ºAno\2ºSemestre\SAD\Trabalho3\file.json";
                string fileContent = File.ReadAllText(fileName);
                richTextBox1.Text = fileContent;
            }

        }

        private void radioButton5_CheckedChanged(object sender, EventArgs e)
        {
            if (radioButton5.Checked)
            {
                button3.Text = "Reset TextBox";
                richTextBox1.Text = printText;
            }

        }




        private void serialPort1_DataSender(String Message)
        {
            // Check if the serial port is open
            if (serialPort1.IsOpen)
            {
                try
                {
                    serialPort1.Write(Message);
                    // Alternatively, you can use WriteLine to send the data with a newline character at the end
                    // serialPort1.WriteLine(configurationData);

                    // Optionally, perform any post-sending operations or update the UI
                    MessageBox.Show("Data sent successfully!");
                }
                catch (Exception ex)
                {
                    // Handle any errors that occur during data sending
                    MessageBox.Show("Error sending data: " + ex.Message);
                }
            }
            else
            {
                MessageBox.Show("Serial port is not open!");
            }
        }

        private StringBuilder receivedDataBuffer = new StringBuilder();

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {

            // Check for a delimiter, such as a newline character, to determine when the complete message has been received
            string receivedData = serialPort1.ReadExisting();

            receivedDataBuffer.Append(receivedData);

            // Check for a delimiter, such as a newline character, to determine when the complete message has been received
            string delimiter = "\n";
            int delimiterIndex = receivedDataBuffer.ToString().IndexOf(delimiter);

            if (delimiterIndex >= 0)
            {
                // Extract the complete message up to the delimiter
                string completeMessage = receivedDataBuffer.ToString().Substring(0, delimiterIndex + delimiter.Length);

                // Remove the processed message from the buffer
                receivedDataBuffer.Remove(0, delimiterIndex + delimiter.Length);

                printText += completeMessage;

                string JSONString = completeMessage.Replace("\n", "");

                Debug.WriteLine( "&" + JSONString + "&");

                JObject jsonObject = JObject.Parse(JSONString);

                //XmlDocument doc = (XmlDocument)JsonConvert.DeserializeXmlNode(completeMessage);
                XmlDocument xmlDoc = new XmlDocument();

                // Create the root element of the XML
                XmlElement rootElement = xmlDoc.CreateElement("57390_57901_57733");
                xmlDoc.AppendChild(rootElement);

                // Loop through the properties of the JSON object
                foreach (JProperty property in jsonObject.Properties())
                {
                    // Check if the property name is "A2" or "A3"
                    if (property.Name == "A2" || property.Name == "A3")
                    {
                        // Create a new XML element with the property name
                        XmlElement element = xmlDoc.CreateElement(property.Name);

                        // Get the array of values from the property value
                        JArray valuesArray = (JArray)property.Value;

                        // Loop through the values and add them as child elements to the XML element
                        foreach (JToken value in valuesArray)
                        {
                            XmlElement valueElement = xmlDoc.CreateElement("Value");
                            valueElement.InnerText = value.ToString();
                            element.AppendChild(valueElement);
                        }

                        // Add the XML element to the root element
                        rootElement.AppendChild(element);
                    }
                }
              
                // Convert the XmlDocument to a string
                string xmlString = xmlDoc.OuterXml;

                SendXmlMessage(xmlString);

                // Process the received JSON data
                try
                {
                    if (radioButton5.Checked)
                    {
                        Invoke((MethodInvoker)delegate
                        {
                            richTextBox1.Text = printText;
                        });
                    }

                    //guardar em ficheiro
                    SaveDataToFile(completeMessage, @"C:\Users\Tiago Zorro\Desktop\Faculdade\4ºAno\2ºSemestre\SAD\Trabalho3\file.json");

                    //enviar para servidor
                    //SendXmlMessage(value);
                }
                catch (JsonException ex)
                {
                    // Handle JSON parsing errors
                    Console.WriteLine("Error parsing JSON: " + ex.Message);
                }
            }
        }
        /*
         
        2023-05-31 02:58:34pm-><message>
            <A2>
                <value>63</value>
                <value>0</value>
                <value>57</value>
            </A2>
            <A5>
                <value>264</value>
                <value>247</value>
                <value>203</value>
            </A5>
        </message>
         */

        /// tem que ser mudado de acordo com o enunciado
        private async void SendXmlMessage(string xmlMessage)
        {
            // Create an HttpClient instance
            using (HttpClient httpClient = new HttpClient())
            {
                // Set the remote server URL
                string serverUrl = "http://193.136.120.133/~sad/";

                try
                {
                    // Create the HTTP content with the XML message
                    HttpContent httpContent = new StringContent(xmlMessage, Encoding.UTF8, "application/xml");

                    // Send the HTTP POST request
                    HttpResponseMessage response = await httpClient.PostAsync(serverUrl, httpContent);

                    // Check the response status
                    if (response.IsSuccessStatusCode)
                    {
                        // Request successful
                        Console.WriteLine("XML message sent successfully!");
                    }
                    else
                    {
                        // Request failed
                        Console.WriteLine("Failed to send XML message. Status Code: " + response.StatusCode);
                    }
                }
                catch (Exception ex)
                {
                    // Handle any exceptions that occur during the HTTP request
                    Console.WriteLine("Error sending XML message: " + ex.Message);
                }
            }

        }

        private void SaveDataToFile(string jsonString, string fileName)
        {
            // Open the file in Append mode
            using (FileStream file = new FileStream(fileName, FileMode.Append))
            {
                using (StreamWriter writer = new StreamWriter(file))
                {
                    // Append the data to the file
                    writer.Write(jsonString);
                }
            }
        }

        private string ReadDataFromFile(string fileName)
        {
            // Open the file in Read mode
            using (FileStream file = new FileStream(fileName, FileMode.Open))
            {
                using (StreamReader reader = new StreamReader(file))
                {
                    // Read the data from the file
                    string fileContent = reader.ReadToEnd();
                    return fileContent;
                }
            }
        }

        private string CleanReceivedData(string data)
        {
            // Replace unwanted characters
            data = data.Replace("\r\n", "\n");
            data = data.Replace("\r", "\n");
            data = data.Replace("\n\n", "\n");

            // Remove any leading or trailing spaces
            data = data.Trim();

            return data;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            serialPort1.PortName = textBox4.Text;
           
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void checkBox8_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox clickedCheckbox = (CheckBox)sender;
            if (clickedCheckbox.Checked)
            {
                digital1 = 1;
            }
            else
            {
                digital1 = 0;
            }
            string send = "{\"D1\":" + digital1 + "}" + '\n';
            richTextBox1.Text = send;
            serialPort1_DataSender(send);
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
