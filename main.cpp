#include <iostream>
#include <random>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fmt/core.h>

using json = nlohmann::json;

using namespace fmt;
using namespace std;

struct RandomRange {
    int start;
    int end;
};


/*
 * Generate a random number from a range
 *
 * @param start - the start of the range
 * @param end   - the end of the range
 *
 * @return the random number between start and end
 */
int randInt (const int start, const int end) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(start,end);
    return static_cast<int>(dist6(rng));
}


/*
 * Translate number of guesses to text
 *
 * @param guesses - the number of guesses
 *
 * @return the string
 */
string guessesToText(const int guesses) {
    if (guesses == 1) return "AMAZING!";
    if (guesses < 5) return "VERY COOL!";
    if (guesses < 15) return "OK.";
    return "Actually pretty bad...";
}

/*
 * Print out a string (idk why i made this, just to add \n)
 *
 * @param txt   - the start of the range
 * @param end   - the end of the range
 *
 * @return the random number between start and end
 */
void pprint (const string& txt = "", const string& end = "\n") {
    cout << txt << end;
}

/*
 * Starts a new guessing game round
 *
 * @param range - the range of the random number
 *
 * @return the number of guesses it took the user
 */
int newGame(const RandomRange range) {
    bool found_number = false;

    // Create the random number
    const int random_num = randInt(range.start, range.end);

    int guesses = 0;

    // Loop until the user guesses the number
    while (!found_number) {
        pprint("Guess the number: ", "");

        // Get the user's input
        string tmp_guess;
        cin >> tmp_guess;
        const int user_guess = stoi(tmp_guess);

        guesses++;

        // Check if the number the user enetered is in the range
        if (user_guess < range.start) {
            pprint(format("The guess is smaller than the start value of the range ({0})", range.start));
            continue;
        }
        if (user_guess > range.end) {
            pprint(format("The guess is bigger than the end value of the range ({0})", range.end));
            continue;
        }

        // If the number the user guessed is equal to the random number, let the user know and stop the loop
        if (user_guess == random_num) {
            pprint(format("You have found the number in only {0} guesses! That's {1}", guesses, guessesToText(guesses)));
            found_number = true;

        // Hint the user towards the correct number
        } else if (user_guess > random_num)
            pprint("The number you have guseed is higher than the random number!");
        else
            pprint("The number you have guseed is lower than the random number!");
    }

    return guesses;
}

/*
 * Get the range from the user
 *
 * @return The RandomRange struct with the range data
 */
RandomRange getRange() {
    string tmp_input;

    while (true) {

        // Get start range
        pprint("What would you like for the start value of the range to be: ", "");
        cin >> tmp_input;
        const int start_range = stoi(tmp_input);

        // Get end range
        pprint("What would you like for the end value of the range to be: ", "");
        cin >> tmp_input;
        const int end_range = stoi(tmp_input);

        // Validate the range
        if (start_range >= end_range) {
            pprint("Invalid range! the start value has to be smaller than the end value.");
        } else {
            return RandomRange {
                .start = start_range,
                .end = end_range
            };
        }
    }


}

/*
 * Reads/creates the leaderboard file and returns the JSON object
 *
 * @return The JSON object
 */
json load_leaderboard () {
    const bool does_lb_exist = std::filesystem::exists("./leaderboard.json");
    if (!does_lb_exist) {

        // If the file doesn't exist, create it with an empty array.
        ofstream lb_file("./leaderboard.json");
        lb_file << "[]";
        lb_file.close();
    }

    // Read and parse the file
    ifstream lb_file("./leaderboard.json");
    json data = json::parse(lb_file);
    return data;
}

/*
 * Get the ordinal suffix for a number (...st,rd,nd,th)
 *
 * @param n - The number
 *
 * @return The suffix for the number
 */
const char* ordinalSuffix(const int n)
{
    static constexpr char suffixes [4][3] = {"th", "st", "nd", "rd"};
    auto ord = n % 100;
    if (ord / 10 == 1) { ord = 0; }
    ord = ord % 10;
    if (ord > 3) { ord = 0; }
    return suffixes[ord];
}

/*
 * Print out the leaderboard file
 *
 * @param data - the JSON object for the leaderboard
 *
 */
void printLeaderboard(json data) {

    // Loop over each player in the leaderboard and print the info
    for(int i = 0; i < data.size(); i++) {
        int place = i+1;
        int score = data[i]["score"];
        string player_name = data[i]["name"];
        print(format("The {0}{1} place is ", place, ordinalSuffix(place)) + player_name + format(" with a score of {0}\n", score));
    }
}

/*
 * Inserts a player JSON object into the leaderboard object in the correct place, and save the new data.
 *
 * @param player - JSON object that contains the player's name and score
 *
 * @return The position of the player (their total place)
 */
int insertPlayer (json player) {

    // Get the leaderboard JSON object
    json data = load_leaderboard();

    // Create an empty JSON array
    json new_data = json::array();

    int player_place = 0;
    bool found_loc = false;

    // Add the player object in the correct location
    for (json p: data) {

        player_place += static_cast<int>(!found_loc); // cool thingy hehe
        if (p["score"] < player["score"]) {
            new_data.push_back(player);
            found_loc = true;
        }
        new_data.push_back(p);
    }

    // If found_loc == false meaning the data is empty, add the player (first place)
    if (!found_loc) {
        new_data.push_back(player);
        player_place++;
    }

    // Convert the JSON to string
    const string json_data = new_data.dump(4);

    // Write the file
    ofstream lb_file("./leaderboard.json");
    lb_file << json_data;
    lb_file.close();

    return player_place;
}


// Check if the player's name already exists
bool doesPlayerExist (const string& name) {
    json data = load_leaderboard();
    for (json p: data) {
        if (p["name"] == name) return true;
    }
    return false;
}

int main() {

    // Print game title
    pprint("WELCOME TO THE RANDOM NUMBER GUESSING GAME!");
    pprint("___________________________________________");
    pprint();

    // Get the user's name
    string username;
    pprint("What is your name: ", "");
    cin >> username;

    // If the name already exists, ask the user for their name again
    while (doesPlayerExist(username)) {
        pprint("Sorry, that name already exists! please try again.");
        pprint("What is your name: ", "");
        cin >> username;
    }

    // Greet the user
    pprint(format("Welcome to my game {0}!", username));

    // Get the user's range
    const RandomRange range = getRange();

    // Start the game
    int guesses = newGame(range);

    // Get the range delta
    int delta_range = range.end - range.start;

    // Calculate the score
    int score = (delta_range / guesses) / 2;

    // Create the player object
    const auto player = json({
        {"name", username},
        {"score", score}
    });

    // Put the player in the leaderboard
    int place = insertPlayer(player);

    // Let the user know their place
    pprint(format("With range of {0} and {1} guesses, your score is: {2} which places you at {3}{4} place", delta_range, guesses, score, place, ordinalSuffix(place)));

    // Ask the user if they want to see the current leaderboard
    string response;
    while (response != "y" && response != "n") {
        pprint("Would you like to see the leaderboard? [y/n]: ");
        cin >> response;
    }

    if (response == "n") {
        pprint("Thanks for playing! goodbye.");
        return 0;
    };

    const auto leaderboard = load_leaderboard();
    printLeaderboard(leaderboard);

    pprint("Press any button to continue...");
    string tmp;
    cin >> tmp;

    return 0;
}
