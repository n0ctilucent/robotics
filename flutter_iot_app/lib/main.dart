import 'dart:async';
import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';
import 'package:provider/provider.dart';

void main() {
  runApp(
    ChangeNotifierProvider(
      create: (_) => MqttProvider(),
      child: const MqttApp(),
    ),
  );
}

class MqttApp extends StatelessWidget {
  const MqttApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ESP32 IoT Controller',
      theme: ThemeData(
        brightness: Brightness.dark,
        primarySwatch: Colors.blue,
        useMaterial3: true,
      ),
      home: const IotDashboard(),
    );
  }
}

class MqttProvider with ChangeNotifier {
  MqttServerClient? client;
  MqttConnectionState connectionState = MqttConnectionState.disconnected;
  String latestMessage = "No messages yet";
  bool ledStatus = false;

  final String brokerIP = '192.168.1.100'; // Change to your local broker IP
  final String clientID = 'flutter_client';
  final String controlTopic = 'esp32/control';
  final String statusTopic = 'esp32/status';

  Future<void> connect() async {
    client = MqttServerClient(brokerIP, clientID);
    client!.port = 1883;
    client!.keepAlivePeriod = 20;
    client!.onDisconnected = onDisconnected;
    client!.onConnected = onConnected;
    client!.onSubscribed = onSubscribed;

    final connMessage = MqttConnectMessage()
        .withClientIdentifier(clientID)
        .startClean()
        .withWillQos(MqttQos.atLeastOnce);
    client!.connectionMessage = connMessage;

    try {
      connectionState = MqttConnectionState.connecting;
      notifyListeners();
      await client!.connect();
    } catch (e) {
      debugPrint('Exception: $e');
      disconnect();
    }
  }

  void disconnect() {
    client?.disconnect();
    connectionState = MqttConnectionState.disconnected;
    notifyListeners();
  }

  void onConnected() {
    connectionState = MqttConnectionState.connected;
    debugPrint('Connected to broker');
    client!.subscribe(statusTopic, MqttQos.atLeastOnce);
    
    client!.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      final recMess = c[0].payload as MqttPublishMessage;
      final pt = MqttPublishPayload.bytesToStringAsString(recMess.payload.message);
      latestMessage = pt;
      
      if (pt == "LED_ON") ledStatus = true;
      if (pt == "LED_OFF") ledStatus = false;
      
      notifyListeners();
    });
    notifyListeners();
  }

  void onDisconnected() {
    connectionState = MqttConnectionState.disconnected;
    notifyListeners();
  }

  void onSubscribed(String topic) {
    debugPrint('Subscribed to topic: $topic');
  }

  void publishMessage(String message) {
    if (connectionState != MqttConnectionState.connected) return;
    final builder = MqttClientPayloadBuilder();
    builder.addString(message);
    client!.publishMessage(controlTopic, MqttQos.atLeastOnce, builder.payload!);
  }

  void toggleLed() {
    ledStatus = !ledStatus;
    publishMessage(ledStatus ? "TOGGLE_ON" : "TOGGLE_OFF");
    notifyListeners();
  }
}

class IotDashboard extends StatelessWidget {
  const IotDashboard({super.key});

  @override
  Widget build(BuildContext context) {
    final provider = Provider.of<MqttProvider>(context);

    return Scaffold(
      appBar: AppBar(
        title: const Text('ESP32 IoT Dashboard'),
        actions: [
          IconButton(
            icon: Icon(provider.connectionState == MqttConnectionState.connected
                ? Icons.cloud_done
                : Icons.cloud_off),
            onPressed: provider.connectionState == MqttConnectionState.connected
                ? provider.disconnect
                : provider.connect,
          )
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            _buildStatusCard(provider),
            const SizedBox(height: 20),
            _buildControlCard(provider),
            const Spacer(),
            Text(
              'Broker IP: ${provider.brokerIP}',
              textAlign: TextAlign.center,
              style: const TextStyle(color: Colors.grey),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildStatusCard(MqttProvider provider) {
    return Card(
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const Text('ESP32 Status', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold)),
            const Divider(),
            Text('Last Message: ${provider.latestMessage}'),
            const SizedBox(height: 10),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                const Text('Connection: '),
                Text(
                  provider.connectionState.toString().split('.').last,
                  style: TextStyle(
                    color: provider.connectionState == MqttConnectionState.connected ? Colors.green : Colors.red,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildControlCard(MqttProvider provider) {
    return Card(
      elevation: 4,
      child: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            const Text('Controls', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold)),
            const SizedBox(height: 20),
            Icon(
              Icons.lightbulb,
              size: 64,
              color: provider.ledStatus ? Colors.yellow : Colors.grey,
            ),
            const SizedBox(height: 10),
            ElevatedButton(
              onPressed: provider.connectionState == MqttConnectionState.connected
                  ? provider.toggleLed
                  : null,
              style: ElevatedButton.styleFrom(
                minimumSize: const Size(200, 50),
              ),
              child: Text(provider.ledStatus ? 'Turn LED OFF' : 'Turn LED ON'),
            ),
          ],
        ),
      ),
    );
  }
}
