#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
#define elem_count 10

int main() {

    int count[elem_count] = {}; // Ініціалізуємо масив кількостей для кожного елемента

    srand(time(NULL)); // Ініціалізуємо генератор випадкових чисел

    for (int i = 0; i < 1000000; i++) {
        int randomIndex = rand() % elem_count; // Генеруємо випадкове число в межах довжини масиву
        count[randomIndex]++;
    }

    for (int i = 0; i < elem_count; i++) {
        cout << "Element " << i + 1 << ": " << count[i] << " times" << endl;
    }

    return 0;
}