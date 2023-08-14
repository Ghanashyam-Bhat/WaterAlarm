

String convertTo12HourFormat(String time24) {
  try {
    DateTime dateTime = DateTime.parse("2000-01-01 $time24");
    String time12 =
        "${(dateTime.hour > 12) ? (dateTime.hour - 12) : dateTime.hour}:${dateTime.minute.toString().padLeft(2, '0')}:${dateTime.second.toString().padLeft(2, '0')} ${(dateTime.hour >= 12) ? 'PM' : 'AM'}";
    return time12;
  } catch (e) {
    print("Error converting time: $e");
    return "Invalid Time";
  }
}


// Helper function to convert numeric day to string representation
String getDayOfWeek(String dayNumber) {
  int index = int.tryParse(dayNumber) ?? 0;
  List<String> days = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
  if (index >= 0 && index < 7) {
    return days[index];
  } else {
    return 'Invalid Day';
  }
}

// Helper function to format date from "dd-MM-yyyy" to a desired format
String formatDate(String date) {
  try {
    List<String> parts = date.split('-');
    if (parts.length == 3) {
      int day = int.tryParse(parts[0]) ?? 0;
      int month = int.tryParse(parts[1]) ?? 0;
      int year = int.tryParse(parts[2]) ?? 0;

      if (day > 0 && month > 0 && year > 0) {
        return "${day.toString().padLeft(2, '0')}-${month.toString().padLeft(2, '0')}-$year";
      }
    }
    return "Invalid Date";
  } catch (e) {
    print("Error formatting date: $e");
    return "Invalid Date";
  }
}
