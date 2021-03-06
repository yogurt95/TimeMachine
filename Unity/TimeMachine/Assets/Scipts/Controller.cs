﻿using System;
using System.Collections;
using Michsky.UI.ModernUIPack;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class Controller : MonoBehaviour
{
    public string ipAddress;
    public int port;

    
    [SerializeField] private string incomingRaw;
    [SerializeField] public float btn1;
    [SerializeField] public float btn2;
    [SerializeField] private float pingTime;


    private bool connected;    
    private readonly WifiConnection _comm = new WifiConnection();
    
    public TextMeshProUGUI statusText;
    public TextMeshProUGUI pingShow;
    private AudioSource player;
    public AudioClip timeMachineSound;

    public TMP_InputField manualIp;
    public TMP_InputField manualPort;

    private bool _showingConnected;

    public Image img;
    private bool userPanelActive = false;

    private void Start()
    {
        player = GetComponent<AudioSource>();
        try
        {
            StartCoroutine(DelayedConnect());
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            throw;
        }
        
        
        //AutomaticConnect(DataTransferClass.staticDataTransferClass.ip,int.Parse(DataTransferClass.staticDataTransferClass.port));
        //ConnectToEsp();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            if (userPanelActive)
            {
                LeanTween.moveLocalX(img.gameObject, 2160, 1).setEase(LeanTweenType.easeInOutQuad);
                userPanelActive = false;
            }
            else
            {
                LeanTween.moveLocalX(img.gameObject, 0, 1).setEase(LeanTweenType.easeInOutQuad);
                userPanelActive = true;
            }
            
        }
        
        if (Input.GetKeyUp(KeyCode.UpArrow) || Input.GetKeyUp(KeyCode.DownArrow))
        {
            _comm.WriteToEsp("Servo:95");
        }

        if (Input.GetKeyDown(KeyCode.UpArrow))
        {
            _comm.WriteToEsp("Servo:1");
        }

        if (Input.GetKeyDown(KeyCode.DownArrow))
        {
            _comm.WriteToEsp("Servo:180");
        }
        
        if (Input.GetKeyDown(KeyCode.Q))
        {
            _comm.StartContinuousPinging();
        }
        if (Input.GetKeyDown(KeyCode.S))
        {
            _comm.StopContinuousPinging();
        }
        if (Input.GetKeyDown(KeyCode.C))
        {
            ConnectToEsp();
        }

        statusText.text = _comm.status;
        connected = _comm.connected;

        incomingRaw = _comm.incomingDataStream;
        btn1 = float.Parse(_comm.incomingDataStream.Split(':')[1]);
        btn2 = float.Parse(_comm.incomingDataStream.Split(':')[2]);
        
        pingTime = _comm.pingTime;
        pingShow.text = pingTime.ToString();
    }

    public void AutomaticConnect(string ip, int port)
    {
        print("Trying to connect automatically");
        if (connected) return;
        
        _comm.Begin(ip,port);
    }

    public void ManualConnect()
    {
        print("Trying to connect manually");
        if (connected)
        {
            return;
        }
        _comm.Begin(manualIp.text, int.Parse(manualPort.text));
    }
    public void ConnectToEsp()
    {
        print("Trying to connect default");
        if (connected)
        {
            return;
        }
        _comm.Begin(ipAddress, port);
    }

    public void DisconnectEsp()
    {
        _comm.CloseConnection("Manually Disconnected");
    }
    

    public void PingDeviceButton()
    {
        _comm.PingDevice();
    }

    public void InitiateTimeTravel()
    {
        player.Play();
        _comm.WriteToEsp("TimeTravel:0");
    }

    private IEnumerator DelayedConnect()
    {
        yield return new WaitForSeconds(3);
        AutomaticConnect(DataTransferClass.staticDataTransferClass.ip,int.Parse(DataTransferClass.staticDataTransferClass.port));
        
    }

     
}