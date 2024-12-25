#include "project-defs.h"

// reads int only not floats
bool read_int(char *line, uint8_t* char_count, int16_t* int_value_ptr) {

  char *ptr = line + *char_count; // the variable that will extract the next digit from
  unsigned char c; // the variable that will hold each digit

  c = *ptr++;  // extracting the next digit!

  // catching negative values :)
  bool isnegative = false;
  if (c == '-') {
    isnegative = true;
    c = *ptr++;

  // incrementing pointer anyway if +ve sign is there
  } else if (c == '+') {
    c = *ptr++;
  }

  // Algorithm to convert char digits to int
  uint32_t intval = 0;
  uint8_t ndigit = 0;  // current digit read index
  int8_t exp = 0;  // NOT uint8_t as this must hold +ve and -ve numbers
  while (1) {

    c -= '0';
    if (c <= 9) {
      ndigit++;
      if (ndigit <= MAX_INT_DIGITS) {

        intval = (((intval << 2) + intval) << 1) + c; // intval*10 + c
                                                      //
      } else {

        exp++;  // Drop overflow digits
      }

    } else {
      // encountered a non integer character
      break;
    }
    c = *ptr++;
  }

  // Return if no digits have been read.
  if (!ndigit) { return(false); };

  long int lival;
  lival = (long int)intval;
  
  if (exp > 0) {
    do {
      lival *= 10.0;
    } while (--exp > 0);
  } 

  // Assign floating point value with correct sign.    
  if (isnegative) {
    *int_value_ptr = -lival;
  } else {
    *int_value_ptr = lival;
  }

  *char_count = ptr - line - 1; // Set char_counter to next statement
 
  return (true);
}

LINE_STATUS terminal_execute_line(char* line) {

  uint8_t char_count = 0;
  char letter;
  // temporary int value that gets read from the terminal then assigned to another variable of any other c component
  uint16_t int_value;

  // To know whether to execute arg func routine or not
  uint8_t mode_func_ind = MODE_FUNC_NONE;

  // ultrasonic stuff
  uint16_t ultrasonic_dist = 0;
#if ULTRASONIC_STATE_MACHINE
  ULTRASONIC_STATUS ultrasonic_current_status = ultrasonic_get_distance(&ultrasonic_dist);
#endif

  while (line[char_count] != 0) {

    // Reading letter argument, aka which function to execute
    letter = line[char_count];

    // incrementing char_count for next iteration
    char_count++;

    // IMP: capital letters are command letters
    //      small letters are argument letters
    // Checking letter is a letter
    // checking if it's before A or after z or any character in between Z and a in the ascii table
    if ( letter < 'A' || letter > 'z' || (letter > 'Z' && letter < 'a')) {
      printf("\rExpected a letter!\n"); 
      return LINE_FAILED;
    }

    /* Identifying function and printing the resultant execution */
    switch(letter) {

      case 'A':
        // testing reading int argument
        if (!read_int(line, &char_count, &int_value)) {
          printf("Bad integer Number Format\n");
          return LINE_FAILED;
        }

        printf("Read INT value: %d\n", int_value);
        break;
   
     default:
        printf("Command Letter Not Implemented\n");
        return LINE_FAILED;
    }

  }
  /* Parsing complete! (and non mode funcs are executed too)*/ 

  // The rest of the steps are only Applicable for Mode funcs
  if (mode_func_ind == MODE_FUNC_NONE) {
    return LINE_PASSED;
  }

  /* [ Step 3: Error checking ] */
  switch(mode_func_ind) {

    case MODE_FUNC_MOVE:
  }

  /* [ Step 4: Execute! ] */
  switch (mode_func_ind) {

    case MODE_FUNC_MOVE:

      break;

    case MODE_FUNC_ULTRASONIC:

  }

  return LINE_PASSED;
}
