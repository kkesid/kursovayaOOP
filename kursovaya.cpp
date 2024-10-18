//Система управления библиотекой позволяет пользователям управлять книгами, пользователями и процессом выдачи книг.
//Программа включает функции для добавления новых книг, регистрации пользователей, выдачи и возврата книг, поиска книг, а также возможность сохранения и загрузки данных о книгах и пользователях из файла.

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

class Book {
private:
    std::string title;
    std::string author;
    std::string isbn;
    bool isAvailable;

public:
    Book(std::string t = "", std::string a = "", std::string i = "")
        : title(t), author(a), isbn(i), isAvailable(true) {}

    void displayInfo() const {
        std::cout << *this; 
    }

    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    void setAvailability(bool availability) { isAvailable = availability; }
    bool isAvailableForBorrow() const { return isAvailable; }

    // Сохранение книги в файл 
    std::string toString() const {
        return title + "," + author + "," + isbn + "," + (isAvailable ? "1" : "0");
    }

    // Загрузка книги из строки
    static Book fromString(const std::string& str) {
        size_t pos1 = str.find(',');
        size_t pos2 = str.find(',', pos1 + 1);
        size_t pos3 = str.find(',', pos2 + 1);

        std::string t = str.substr(0, pos1);
        std::string a = str.substr(pos1 + 1, pos2 - pos1 - 1);
        std::string i = str.substr(pos2 + 1, pos3 - pos2 - 1);
        bool available = str.substr(pos3 + 1) == "1";

        Book book(t, a, i);
        book.setAvailability(available);
        return book;
    }

    friend std::ostream& operator<<(std::ostream& os, const Book& book) {
        os << "Title: " << book.title << ", Author: " << book.author << ", ISBN: " << book.isbn << ", Available: "
            << (book.isAvailable ? "Yes" : "No") << std::endl;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Book& book) {
        std::cout << "Enter title: ";
        is >> std::ws; 
        std::getline(is, book.title);
        std::cout << "Enter author: ";
        std::getline(is, book.author);
        std::cout << "Enter ISBN: ";
        std::getline(is, book.isbn);
        return is;
    }
};

class User {
private:
    std::string name;
    std::string userId;
    std::vector<Book*> borrowedBooks;

public:
    User(const std::string& n = "", const std::string& id = "") : name(n), userId(id) {}

    std::string getUserId() const { return userId; }

    void borrowBook(Book& book) {
        if (book.isAvailableForBorrow()) {
            borrowedBooks.push_back(&book);
            book.setAvailability(false);
            std::cout << name << " borrowed \"" << book.getTitle() << "\"." << std::endl;
        }
        else {
            std::cout << "Book is not available." << std::endl;
        }
    }

    void returnBook(Book& book) {
        auto it = std::find(borrowedBooks.begin(), borrowedBooks.end(), &book);
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
            book.setAvailability(true);
            std::cout << name << " returned \"" << book.getTitle() << "\"." << std::endl;
        }
        else {
            std::cout << "This book was not borrowed by " << name << "." << std::endl;
        }
    }

    void displayBorrowedBooks() const {
        std::cout << name << "'s borrowed books:" << std::endl;
        for (auto book : borrowedBooks) {
            book->displayInfo();
        }
    }

    // Сохранение пользователя в файл
    std::string toString() const {
        return name + "," + userId;
    }

    // Загрузка пользователя из строки 
    static User fromString(const std::string& str) {
        size_t pos = str.find(',');
        return User(str.substr(0, pos), str.substr(pos + 1));
    }

    friend std::ostream& operator<<(std::ostream& os, const User& user) {
        os << user.toString();
        return os;
    }

    friend std::istream& operator>>(std::istream& is, User& user) {
        std::cout << "Enter user name: ";
        is >> std::ws;
        std::getline(is, user.name);
        std::cout << "Enter user ID: ";
        std::getline(is, user.userId);
        return is;
    }
};

class Library {
private:
    std::vector<Book> books;
    std::vector<User> users;

public:
    void addBook(const Book& book) {
        books.push_back(book);
    }

    void registerUser(const User& user) {
        for (const auto& existingUser : users) {
            if (existingUser.getUserId() == user.getUserId()) {
                std::cout << "User  ID already exists. Please choose a different ID." << std::endl;
                return;
            }
        }
        users.push_back(user);
    }

    Book* findBookByTitle(const std::string& title) {
        for (auto& book : books) {
            if (book.getTitle() == title) {
                return &book;
            }
        }
        return nullptr;
    }

    // Поиск книг по автору 
    std::vector<Book*> findBooksByAuthor(const std::string& author) {
        std::vector<Book*> result;
        for (auto& book : books) {
            if (book.getAuthor() == author) {
                result.push_back(&book);
            }
        }
        return result;
    }

    void issueBook(const std::string& userId, const std::string& bookTitle) {
        for (auto& user : users) {
            if (userId == user.getUserId()) {
                Book* book = findBookByTitle(bookTitle);
                if (book) {
                    user.borrowBook(*book);
                }
                else {
                    std::cout << "Book not found." << std::endl;
                }
                return;
            }
        }
        std::cout << "User  not found." << std::endl;
    }

    void returnBook(const std::string& userId, const std::string& bookTitle) {
        for (auto& user : users) {
            if (userId == user.getUserId()) {
                Book* book = findBookByTitle(bookTitle);
                if (book) {
                    user.returnBook(*book);
                }
                else {
                    std::cout << "Book not found." << std::endl;
                }
                return;
            }
        }
        std::cout << "User  not found." << std::endl;
    }

    User* findUserById(const std::string& userId) {
        for (auto& user : users) {
            if (user.getUserId() == userId) {
                return &user;
            }
        }
        return nullptr;
    }

    // Сохранение данных в файл 
    void saveData(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;

        for (const auto& book : books) {
            file << book.toString() << "\n";
        }

        for (const auto& user : users) {
            file << user.toString() << "\n";
        }

        file.close();
    }

    // Загрузка данных из файла
    void loadData(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        books.clear();
        users.clear();

        std::string line;

        while (std::getline(file, line)) {
            if (line.empty()) continue; // Пропускаем пустые строки
            if (line.find(',') == line.rfind(',')) { // Если есть только 2 запятые, это пользователь 
                users.push_back(User::fromString(line));
            }
            else { // Иначе это книга
                books.push_back(Book::fromString(line));
            }
        }
        file.close();
    }
};

void displayMenu() {
    std::cout << "Menu:\n";
    std::cout << "1. Add Book\n";
    std::cout << "2. Register User\n";
    std::cout << "3. Issue Book\n";
    std::cout << "4. Return Book\n";
    std::cout << "5. Display Borrowed Books\n";
    std::cout << "6. Find Books by Author\n";
    std::cout << "7. Save Data\n";
    std::cout << "8. Load Data\n";
    std::cout << "9. Exit\n";
    std::cout << "Choose an option: ";
}

int main() {
    Library library;
    int choice;

    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cout << std::endl;

        switch (choice) {
        case 1: {
            Book book;
            std::cin.ignore();
            std::cin >> book; 
            library.addBook(book);
            std::cout << "Book added successfully.\n";
            break;
        }
        case 2: {
            User user;
            std::cin.ignore();  
            std::cin >> user;
            library.registerUser(user); 
            std::cout << "User  registered successfully.\n";
            break;
        }
        case 3: {
            std::string userId, bookTitle;
            std::cout << "Enter user ID: ";
            std::cin.ignore();
            std::getline(std::cin, userId);
            std::cout << "Enter book title: ";
            std::getline(std::cin, bookTitle);
            library.issueBook(userId, bookTitle);
            break;
        }
        case 4: {
            std::string userId, bookTitle;
            std::cout << "Enter user ID: ";
            std::cin.ignore();
            std::getline(std::cin, userId);
            std::cout << "Enter book title: ";
            std::getline(std::cin, bookTitle);
            library.returnBook(userId, bookTitle);
            break;
        }
        case 5: {
            std::string userId;
            std::cout << "Enter user ID: ";
            std::cin.ignore();
            std::getline(std::cin, userId);
            User* user = library.findUserById(userId);
            if (user) {
                user->displayBorrowedBooks();
            }
            else {
                std::cout << "User  not found." << std::endl;
            }
            break;
        }
        case 6: {
            std::string author;
            std::cout << "Enter author name: ";
            std::cin.ignore();
            std::getline(std::cin, author);
            std::vector<Book*> books = library.findBooksByAuthor(author);
            if (books.empty()) {
                std::cout << "No books found by this author.\n";
            }
            else {
                for (auto book : books) {
                    book->displayInfo();
                }
            }
            break;
        }
        case 7: {
            std::string filename;
            std::cout << "Enter filename to save data: ";
            std::cin.ignore();
            std::getline(std::cin, filename);
            library.saveData(filename);
            std::cout << "Data saved successfully.\n";
            break;
        }
        case 8: {
            std::string filename;
            std::cout << "Enter filename to load data: ";
            std::cin.ignore();
            std::getline(std::cin, filename);
            library.loadData(filename);
            std::cout << "Data loaded successfully.\n";
            break;
        }
        case 9: {
            std::cout << "Exiting...\n";
            return 0;
        }
        default: {
            std::cout << "Invalid option. Please try again.\n";
            break;
        }
        }
        system("pause");
        system("cls");
    }

    return 0;
}

