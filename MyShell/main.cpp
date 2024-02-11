#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm> 
#include <fstream>
#include <sstream>
#include <set>
using namespace std;

class Abstract : public enable_shared_from_this<Abstract>{
protected:
    string name;
public:
    virtual ~Abstract() = default;
    virtual void listContents() const = 0;                              // ls
    virtual void makeDirectory(const shared_ptr<Abstract>& child) = 0;  // mkdir
    virtual void removeFile(const string& fileName) = 0;                // rm
    virtual shared_ptr<Abstract> changeDirectory(const string& newDir)=0;              // cd
    virtual void displayContent() const = 0;                            // cat
    virtual shared_ptr<Abstract> findFile(const string& fileName) const=0;    //cp
    virtual shared_ptr<Abstract> findFileRecursive(const string& fileName) const = 0;
    virtual bool isDirectory() const =0;                //
    virtual string getName() const =0;                  //
    virtual void setName(const string& name_) = 0;
    virtual void setParent(const shared_ptr<Abstract>& p) =0;                   //
    virtual const vector<shared_ptr<Abstract>>& getContents() const =0;
    virtual const string& getContent() const =0;                //
    virtual char getType() const = 0;
    virtual shared_ptr<Abstract> findFileByName(const string& fileName) const = 0;
    
};
class Directory : public Abstract{

protected:
    string name;
    weak_ptr<Abstract> parent;
    vector<shared_ptr<Abstract> > contents;

public:
    Directory(const string& value) : name(value) { }
    Directory() : name("root") { }

    void makeDirectory(const shared_ptr<Abstract>& node) {
        if (node) {
            const string& name = node->getName();
            if (!findFile(name)) {
                contents.push_back(node);
            } else {
                cout << "A file or directory with the name " << name << " already exists." << endl;
            }
        }
    }
    shared_ptr<Abstract> getParent() {
        auto parentDirectory = parent.lock();
        if (parentDirectory) {
            auto parentDir = dynamic_pointer_cast<Directory>(parentDirectory);
            if(parentDir){
                cout << "DEBUG: changed directory to parent " << parentDir->getName() << endl;
                return parentDir->changeToParentDirectory();
            }else{
                cout << "DEBUG: Error: Parent isnt a directory\n";
                return parentDirectory;
            }
        } else {
            cout << "DEBUG: Already in the root directory\n";
            return shared_from_this();
        }
    }  
    void setParent(const shared_ptr<Abstract>& p) override {
        parent = p;                                        
    }
    const vector<shared_ptr<Abstract> >& getContents() const{
        return contents;
    }
    void setName(const string& name_)override {
        name = name_;
    } 
    void listContents() const override {
        cout << "List of current directory " << name << " is: \n";

        for (size_t i = 0; i < contents.size(); ++i) {
            for (size_t j = 0; j <= i; ++j) {
                cout << contents[j]->getName() << " ";
            }
            cout << endl;
        }
    }
    void displayContent() const override {
        cout << "Error: Not a regular file.\n";
    } 
    shared_ptr<Abstract> findFileRecursive(const string& fileName) const override {
        auto it = find_if(contents.begin(), contents.end(), [&](const shared_ptr<Abstract>& item) {
            return item->getName() == fileName &&  !item->isDirectory();
        });

        if (it != contents.end()) {
            return *it;
        }
        for (const auto& item : contents) {
            if (item->isDirectory()) {
                auto subdir = dynamic_pointer_cast<const Directory>(item);                      
                if (subdir) {
                    auto result = subdir->findFileRecursive(fileName);
                    if (result) {
                        return result;
                    }
                }
            }
        }
        return nullptr;
    }
    string getName() const override {
        return name;
    }
    bool isDirectory() const override {
        return true;
    }
    const string& getContent() const override {
        static const string emptyContent;  
        return emptyContent;
    }
    shared_ptr<Abstract> changeDirectory(const string& nameDir) {
        auto it = find_if(contents.begin(), contents.end(), [&](const shared_ptr<Abstract>& item) {
            return item->isDirectory() && item->getName() == nameDir;
        });

        if (it != contents.end()) {
            auto newDirectory = dynamic_pointer_cast<Directory>(*it);
            return newDirectory;
        } else {
            return shared_from_this();
        }
        return shared_from_this();
    }  
    shared_ptr<Abstract> changeToCurrentDirectory(){
        return shared_from_this();
    } 
    shared_ptr<Abstract> changeToParentDirectory(){
        auto parentDirectory = getParent();
        
        if(parentDirectory ){
            return parentDirectory;
        }
        else{
            return shared_from_this();
        }
    }
    shared_ptr<Abstract> findFile(const string& fileName) const {
        auto root =shared_from_this();
        return root->findFileRecursive(fileName);
    }
    void removeFile(const string& fileName) override{
        auto it = find_if(contents.begin(), contents.end(), [&](const shared_ptr<Abstract>& item){
            return item->getName() == fileName && !item->isDirectory();
        });
        if(it != contents.end()){
            contents.erase(it);
            cout << "File " << fileName << " removed from " << name << endl;
        }else{
            cout << "Error: File " << fileName << " not found in the current directory.\n";
        }
    }
    char getType() const override {
        return isDirectory() ? 'D' : 'F';
    }
    shared_ptr<Abstract> findFileByName(const string& fileName) const override {
        auto it = find_if(contents.begin(), contents.end(), [&](const shared_ptr<Abstract>& item) {
            return item->getName() == fileName;
        });

        return (it != contents.end()) ? *it : nullptr;
    }
};

class File : public Abstract  {
private:
    string name2;
    string content = "shantal";                                                   //it is my cat's name :)
    string soft_link;
    bool isSoftLink=false;
    string softLinkName;  

public:
    File(const string& name) : name2(name) { }

    void listContents() const override {
        cout << "File: " << name2 << "  ";
    }
    bool isDirectory() const override {
        return false;
    }
    void makeDirectory(const shared_ptr<Abstract>& child) override {
        cout << "Error: Cannot add a directory to a file.\n";
    }
    void removeFile(const string& fileName) override {
        cout << "Error: Cannot remove a file directly. Use the 'rm' command from a directory.\n";
    }
    string getName() const override{
        return name2;
    } 
    shared_ptr<Abstract> changeDirectory(const string& nameDir) override {
        cout << "Error: Cannot change directory on a file.\n";
        return shared_from_this(); 
    }
    void setContent(const string& newContent) {
        content = newContent;
    }
    const vector<shared_ptr<Abstract>>& getContents() const override {
        static const vector<shared_ptr<Abstract> > emptyContents;
        return emptyContents;
    }
    void displayContent() const override{
        cout << "Content of file " << name2 << ": " << content << endl;
    }
    void setName(const string& name_){
        name2 = name_;
    }
    shared_ptr<Abstract> findFile(const string& fileName) const override {
        return nullptr;
    }
    void setParent(const shared_ptr<Abstract>& p){
        cout << "This function is not for File class\n";
    }
    const string& getContent() const override {
        return content;
    }
    shared_ptr<Abstract> findFileRecursive(const string& fileName) const override {
        return nullptr;
    }
    void copyTo(const string& sourceFilePath, const string& destinationFileName ,const shared_ptr<Abstract>& currentDirectory) {
        ifstream sourceFile(sourceFilePath, ios::binary);
        if (!sourceFile) {
            cerr << "Error: Unable to open source file: " << sourceFilePath << endl;
            return;
        }
        string content((istreambuf_iterator<char>(sourceFile)), istreambuf_iterator<char>());
        
        auto it = find_if(currentDirectory->getContents().begin(), currentDirectory->getContents().end(),
            [&](const shared_ptr<Abstract>& item){
                return item->getName() == destinationFileName;
        });

        if(it != currentDirectory->getContents().end() && !(*it)->isDirectory()){
            auto destinationFile = dynamic_pointer_cast<File>(*it);
            if(destinationFile){
                destinationFile->setContent(content);
                cout << "File copied to: " << currentDirectory->getName() << "/" << destinationFile->getName() << endl;
            }else{
                cerr << "Error: Destination is not a file.\n";
            }
        }else{
            auto destinationFile = make_shared<File>(destinationFileName);
            destinationFile->setContent(content);
            currentDirectory->makeDirectory(destinationFile);
            cout << "File copied to: " << currentDirectory->getName() << "/" << destinationFileName << endl;
        }
    }
    void printTreeToFile(const string& fileName) const {
        cout << "This function is for this class.\n";
    }
    char getType() const override {
        return isDirectory() ? 'D' : 'F';
    } 
    shared_ptr<Abstract> findFileByName(const string& fileName) const override {
        if (name == fileName) {
            return const_pointer_cast<Abstract>(shared_from_this());
        } else {
            return nullptr;
        }
    }

    void createSoftLink(const shared_ptr<File>& sourceFile, const string& linkName) {
        if (sourceFile) {
            setContent(sourceFile->getContent());
            soft_link= sourceFile->getContent();
            softLinkName = linkName;
            cout << "Soft link created with content from: " << sourceFile->getName() << endl;
        } else {
            cerr << "Error: Soft link is not properly set up." << endl;
        }
    }
    void updateSoftLink() {
        if (isSoftLink) {
            setContent(soft_link);
            cout << "Soft link updated with content from: " << getName() << " to " << softLinkName << endl;
        } else {
            cout << "Error: Soft link is not properly set up." << endl;
        }
    }

    void printNodeContent(ostream& outFile, int level = 0) const {
        for (int i = 0; i < level; ++i) {
            outFile << "  "; 
        }

        outFile << getType() << " " << name2;
        if (isSoftLink) {
            outFile << "L " << soft_link;
        } else if (getType() == 'F') {
            outFile << " {" << content << "}";
        }
        outFile << endl;
    }
    bool getIsSoftLink() const {
        return isSoftLink;
    }

    void setIsSoftLink(bool value) {
        isSoftLink = value;
    }
    void setSoftLink(const string& sourceFileName) {
        softLinkName= sourceFileName;
    }
    const string& getSoftLink() const {
        return soft_link;
    }
    string getSoftLinkName() const {
        return softLinkName;
    }
};
class SoftLink : public Abstract {
private:
    shared_ptr<File> sourceFile;
    string name3, content2;

public:
    SoftLink(const shared_ptr<File>& source) : sourceFile(source) {}

    void listContents() const override {
        cout << "Soft Link: " << sourceFile->getName() << "L " << getName() << " ";
    }
    void makeDirectory(const shared_ptr<Abstract>& child){
        cout << "Not valid function.\n";
    }
    void removeFile(const string& fileName){
        cout << "Not valid function.\n";
    }             
    shared_ptr<Abstract> changeDirectory(const string& newDir){
        cout << "Error: Cannot change directory on a soft linked file.\n";
        return shared_from_this();
    }
    void displayContent() const{
        cout << "Not valid function.\n";
    }
    shared_ptr<Abstract> findFile(const string& fileName)const {
        return nullptr;
    }  
    shared_ptr<Abstract> findFileRecursive(const string& fileName) const{
        return nullptr;
    }

    bool isDirectory()const {
        return false;
    }             
    string getName() const {
        return name3;
    }             
    void setName(const string& name_){
        cout << "Not valid function.\n";
    }
    void setParent(const shared_ptr<Abstract>& p){
        cout << "Not valid function.\n";
    }           
    const vector<shared_ptr<Abstract>>& getContents() const{
        static const vector<shared_ptr<Abstract> > emptyContents;
        return emptyContents;
    }
    const string& getContent() const{
        return content2;
    }              
    char getType() const {
        return 'L';
    }
    shared_ptr<Abstract> findFileByName(const string& fileName) const {
        if (name3 == fileName) {
            return const_pointer_cast<Abstract>(shared_from_this());
        } else {
            return nullptr;
        }
    }
    
    void printNodeContent(ostream& outFile, int level = 0) const {
        for (int i = 0; i < level; ++i) {
            outFile << "  "; 
        }
        outFile << getType() << " " << sourceFile->getName() << "L " << getName() << endl;
    } 
};

void printFileHierarchyToFile(const shared_ptr<Abstract>& root, ofstream& outFile, int level = 0) {
    if (root == nullptr) {
        return;
    }

    for (int i = 0; i < level; ++i) {
        outFile << "  "; 
    }
    outFile << root->getType() << " " << root->getName();

    if (auto fileNode = dynamic_pointer_cast<File>(root)) {
        if (fileNode->getIsSoftLink()) {
            outFile << "L " << fileNode->getSoftLinkName();
        } else {
            outFile << " {" << fileNode->getContent() << "}";
        }
    }
    outFile << endl;

    auto directoryNode = dynamic_pointer_cast<Directory>(root);
    if (directoryNode) {
        for (const auto& child : directoryNode->getContents()) {
            printFileHierarchyToFile(child, outFile, level + 1);
        }
    }
}

shared_ptr<Directory> fillFileHierarchyFromFile(ifstream& inFile, bool isTopLevel = true) {
    string line;
    shared_ptr<Directory> root = make_shared<Directory>("root");
    vector<pair<shared_ptr<Directory>, int>> directoryStack;
    directoryStack.push_back({root, -1});  

    while (getline(inFile, line)) {
        istringstream iss(line);
        char type;
        string name, content;

        iss >> type >> name;

        shared_ptr<Abstract> node;
        if (type == 'D') {
            if (name == "root") {
                continue;
            }
            node = make_shared<Directory>(name);
        } else if (type == 'F') {
            iss >> ws;  
            if (iss.peek() == '{') {
                iss.ignore(); 
                getline(iss, content, '}');
            }
            node = make_shared<File>(name);
            auto fileNode = dynamic_pointer_cast<File>(node);
            fileNode->setContent(content);
        } else if (type == 'L') {
            string linkName;
            iss >> linkName;
            auto sourceFile = dynamic_pointer_cast<File>(root->findFileByName(linkName));
            node = make_shared<SoftLink>(sourceFile);
        }

        int currentLevel = 0;
        while (line[currentLevel * 2] == ' ') {
            ++currentLevel;
        }
        while (currentLevel <= directoryStack.back().second) {
            directoryStack.pop_back();
        }
        auto currentDirectory = directoryStack.back().first;
        currentDirectory->makeDirectory(node);

        if (type == 'D') {
            auto directoryNode = dynamic_pointer_cast<Directory>(node);
            if (directoryNode) {
                directoryStack.push_back({directoryNode, currentLevel});
            }
        }
        if (isTopLevel) {
            cout << "New directory or file added to root: " << name << endl;
        }
    }

    return root;
}

void printTree(const shared_ptr<Abstract>& root, int level = 0) {
    if (root) {
        for (int i = 0; i < level; ++i) {
            cout << "  "; 
        }

        cout << root->getName();
        if (root->isDirectory()) {
            cout << "/";
        }
        cout << endl;

        auto directoryNode = dynamic_pointer_cast<Directory>(root);
        if (directoryNode) {
            for (const auto& child : directoryNode->getContents()) {
                printTree(child, level + 1);
            }
        }
    }
}

int main() {
    ifstream inFile("file_hierarchy.txt");
    auto root = fillFileHierarchyFromFile(inFile);
    shared_ptr<Directory> currentDirectory = root;
    inFile.close();
    
    while(true){
        cout << "\nEnter a command: ";
        string command;
        cin >> command;
        if(command == "ls"){
            currentDirectory->listContents();
            //cout << currentDirectory->getName() << endl << endl;
        }
        else if(command == "mkdir"){
            cout << "Enter a directory name: ";
            string dirName;
            cin >> dirName;
            currentDirectory->makeDirectory(make_shared<Directory>(dirName));
        }
        else if( command == "rm"){

            cout << "Enter file name: ";
            string fileName;
            cin >> fileName;
            currentDirectory->removeFile(fileName);
        }
        else if(command == "cd" ){

            cout << "Enter directory name: ";
            string dirName;
            cin >> dirName;
            currentDirectory = dynamic_pointer_cast<Directory>(currentDirectory->changeDirectory(dirName));
            //cout << currentDirectory->getName() << endl << endl;
        }
        else if(command == "cd." ){
            currentDirectory = dynamic_pointer_cast<Directory>(currentDirectory->changeToCurrentDirectory());
        }
        /*else if(command == "cd.."){                                   
            cout << currentDirectory->getName() << endl;
            //currentDirectory = dynamic_pointer_cast<Directory>(currentDirectory->changeToParentDirectory());
            //cout << currentDirectory->getName() << endl;
            auto newDirectory = dynamic_pointer_cast<Directory>(currentDirectory->changeToParentDirectory());

            if (newDirectory && newDirectory != currentDirectory) {
                currentDirectory = newDirectory;
                cout << currentDirectory->getName() << endl;
            } else {
                cout << "Already in the root directory\n";
            }
            cout << currentDirectory->getName() << endl;
        }*/
        else if(command == "cat"){
            cout << "Enter file name: ";
            string fileName;
            cin >> fileName;
            auto it = find_if(currentDirectory->getContents().begin(), currentDirectory->getContents().end(),
            [&](const shared_ptr<Abstract>& item){
                return item->getName() == fileName;
            });

            if(it != currentDirectory->getContents().end()  && !(*it)->isDirectory()){
                (*it)->displayContent();
            }else{
                cout << "Error: File not found\n";
            }
        }
        else if(command == "cp"){                       //TO USE-> /home/slakdn/Masaüstü/hw2/history.txt
            cout << "Enter source file path: ";
            string sourceFilePath, destinationFileName;
            cin >> sourceFilePath;
    
            cout << "Enter destination file name: ";
            cin >> destinationFileName;
            auto fileInstance = make_shared<File>("");
            fileInstance->copyTo(sourceFilePath, destinationFileName, currentDirectory);
        }
        else if(command == "link"){
            cout << "Enter file name to link: ";
            string fileNameToLink;
            cin >> fileNameToLink;

            auto fileToLink = dynamic_pointer_cast<File>(currentDirectory->findFile(fileNameToLink));
            if (fileToLink) {
                if (!fileToLink->getIsSoftLink()) {
                    cout << "Enter link name: ";
                    string linkName;
                    cin >> linkName;
                    fileToLink->createSoftLink(fileToLink, linkName);
                } else {
                    fileToLink->updateSoftLink();
                }
    } else {
        cerr << "Error: File not found." << endl;
    }
        }
        else if(command == "exit"){
            ofstream outFile("file_hierarchy.txt");
            printFileHierarchyToFile(root, outFile,0);
            outFile.close();
            break;
        }
        else{
            cout << "Invalid command. Try again\n";
        }
    }
    return 0;
}