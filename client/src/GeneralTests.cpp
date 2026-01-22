#include "../include/StompProtocol.h"
#include "../include/event.h"
#include <stdlib.h>
#include <string>

int main(int argc, char* argv[]) {
    names_and_events parsed = parseEventsFile("data/events4.json");
    std::string gameName = parsed.team_a_name + "_" + parsed.team_b_name;
    Game game = Game(gameName);

    Event event1 = parsed.events[0];
    Event event2 = parsed.events[1];
    Event event3 = parsed.events[2];

    event1.set_username("Ishai");
    event2.set_username("Tamir");
    event3.set_username("Meni");
    event3.make_second_half_time();
    std::cout << "&&&&&&&& compareTo TO TEST &&&&&&&&&&&" << std::endl;
    std::cout << "Should be true: " << event1.compareTo(event2) << std::endl;
    std::cout << "Should be false: " << event2.compareTo(event1) << std::endl;
    // std::cout << "Should be 0: " << event2.compareToTo(event2) << std::endl;
    std::cout << "Should be false: " << event3.compareTo(event2) << std::endl;
    std::cout << "Should be true: " << event2.compareTo(event3) << std::endl;

    std::cout << "&&&&&&&&&&&&&&& Game::addEvents test &&&&&&&&&&&&&&&" << std::endl;
    std::string dat1
        = "username: Ishai\nteam a: Germany\nteam b: Japan\nevent name: kickoff\ntime: 10\ngeneral game updates:\n    "
          "active: true\n    before halftime: true\nteam a updates:\n   goals: 1\nteam b updates:\n    goals: "
          "2\ndescription: Message 1\n\n";

    // std::string dat2
    //     = "username: Meni\nteam a: Germany\nteam b: Japan\nevent name: kickoff3\ntime: 25\ngeneral game updates:\n "
    //       "active: true\n    before halftime: false\nteam a updates:\n   goals: 4\nteam b updates:\ndescription:
    //       Message 3\n\n";

    std::string dat2 = "username: Ishai\nteam a: Germany\nteam b: Japan\nevent name: kickoff3\ntime: 25\ngeneral game "
                       "updates:\n    active: true\n    before halftime: false\nteam a updates:\n   goals: 4\nteam b "
                       "updates:\ndescription: Message 3\n\n";

    std::string dat3
        = "username: Tamir\nteam a: Germany\nteam b: Japan\nevent name: kickoff2\ntime: 30\ngeneral game updates:\n    "
          "active: true\n    before halftime: true\nteam a updates:\n   goals: 3\nteam b updates: 3\ndescription: "
          "Message 2\n\n";

    Game game2 = Game("Germany_Japan");
    std::cout << "Before add events" << std::endl;
    game2.addEvent(dat3);
    game2.addEvent(dat1);
    game2.addEvent(dat2);
    std::cout << "After add events" << std::endl;
    std::cout << game2.print_events() << std::endl;

    std::cout << game2.summarize("Ishai") << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << game2.summarize("Tamir") << std::endl;
}