#include "Json.h"

using namespace std;
int main() {
//    auto js = Json("{\"dorm142\" : ["
//                 "\"Lyh\" ,"
//                 "\"Fxw\" ,"
//                 "\"Lty\" ,"
//                 "\"Mmx\"\n"
//                 "] , \"asg\": null   , \"lkt\" : [], \"wzy\": \"\"}");
//    cout << js <<endl;
//    cout << js["asg"] << endl;

    auto js2 = createJsonWithFile("/Users/fuasahi/Desktop/twitter.json");
    cout << js2 << endl;

    printf("wuwuwu\n");
}