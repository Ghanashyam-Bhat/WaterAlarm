import 'package:assets_audio_player/assets_audio_player.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'datetime_format.dart';
import 'firebase_options.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp(
    options: DefaultFirebaseOptions.currentPlatform,
  );
  runApp(MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {

  final ValueNotifier<bool> soundEnabled = ValueNotifier<bool>(false);
  var firebaseStream = FirebaseFirestore.instance.collection("water_tank").snapshots();

  final assetsAudioPlayer = AssetsAudioPlayer();

  void _playSound() async{
    if(soundEnabled.value==true){
      print("PLAYER SHOULD PLAY");
      assetsAudioPlayer.open(
        Audio("assets/sound.mp3"),
        autoStart: true
      );
    }else{
      assetsAudioPlayer.stop();
    }
  }



  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: "Water Alarm",
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: Scaffold(
        appBar: AppBar(
          title: Text("Water Level Indicator"),
          actions: [
            ValueListenableBuilder<bool>(
              valueListenable: soundEnabled,
              builder: (context, value, child) {
                return Switch(
                  value: value,
                  onChanged: (newValue) {
                    setState(() {
                      soundEnabled.value = newValue; // Update the state using setState
                    });
                  },
                );
              },
            ),
          ],
        ),
        body: StreamBuilder(
          stream: firebaseStream,
          builder: (ctx, AsyncSnapshot<QuerySnapshot<Map<String, dynamic>>> snapshot) {
            if (snapshot.connectionState == ConnectionState.waiting) {
              return const Center(
                child: CircularProgressIndicator(),
              );
            } else {
              // Assuming there's a single document with id "tank" in the collection
              var doc = snapshot.data?.docs.first;
              var status = doc?['status'];
              var day = getDayOfWeek(doc?['day']);
              var time24 = doc?['time'];
              var time12 = convertTo12HourFormat(time24);
              var date = doc?['date']; // Assuming the field is named "date" in the Firestore document
              var formattedDate = formatDate(date);

              if (status=="HIGH"){
                print("FUNCTION CALLED");
                _playSound();
              }
              else{
                assetsAudioPlayer.stop();
              }
              // Display the data in a visually appealing layout
              return Padding(
                padding: const EdgeInsets.all(20),
                child: Column(
                  mainAxisAlignment: MainAxisAlignment.center,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: <Widget>[
                    AnimatedContainer(
                      duration: Duration(milliseconds: 500),
                      width: 120,
                      height: 120,
                      decoration: BoxDecoration(
                        shape: BoxShape.circle,
                        color: (status == 'HIGH') ? Colors.green : Colors.red,
                      ),
                      child: Center(
                        child: Text(
                          'Tank Status',
                          style: TextStyle(
                            color: Colors.white,
                            fontSize: 16,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                      ),
                    ),
                    SizedBox(height: 20),
                    Container(
                      child: Text(
                        'Date: $formattedDate',
                        style: TextStyle(
                          fontSize: 18,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                    ),
                    SizedBox(height: 10),
                    Container(
                      child: Text(
                        'Day: $day',
                        style: TextStyle(
                          fontSize: 18,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                    ),
                    SizedBox(height: 10),
                    Container(
                    child: Text(
                    'Time: $time12',
                    style: TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                    ),
                    ),
                    ),
                    SizedBox(height: 20),
                    soundEnabled.value==true?
                    Text(
                      'Sound Alert: ON',
                      style: TextStyle(
                        color: Colors.blueAccent,
                        fontSize: 18,
                        fontWeight: FontWeight.bold,
                      ),
                    ):Text(
                      'Sound Alert: OFF',
                      style: TextStyle(
                        color: Colors.red,
                        fontSize: 18,
                        fontWeight: FontWeight.bold,
                      ),
                    )
                  ],
                ),
              );
            }
          },
        ),
      ),
    );
  }
}
