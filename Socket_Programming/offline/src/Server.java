import java.io.File;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;

public class Server {
    private static int usedBuffer = 0;
    private static ArrayList<String> connectedUsers;
    private static ArrayList<String> activeUsers;
    private static HashMap<String, String> FileRequest = new HashMap<>();
    private static HashMap<String,ArrayList<String>> clientInbox = new HashMap<>();
    private static HashMap<String, String> fileMap = new HashMap<>();

    public static void serve(Socket clientSocket) {
        Thread clientthread = new Worker( clientSocket);
        clientthread.start();
    }
    public static ArrayList<String> getConnectedUsers(){
        return connectedUsers;
    }

    public static ArrayList<String> getActiveUsers(){
        return activeUsers;
    }

    public int getMaxChunkSize(){
        return 1000;
    }
    public int getMaxBufferSize(){
        return 1000000;
    }

    public int getMinChunkSize(){
        return 5;
    }

    public int getUsedBuffer(){
        return usedBuffer;
    }
    public static void UseBuffer(int amount){
        usedBuffer+=amount;
    }
    public HashMap<String, String> getFileRequest() {
        return FileRequest;
    }

    public HashMap<String, ArrayList<String>> getClientInbox(){
        return clientInbox;
    }

    public HashMap<String, String> getFileMap(){
        return fileMap;
    }

    public static void main(String[] args) {
        try{
            ServerSocket serverSocket = new ServerSocket(6666);
            connectedUsers = new ArrayList<>();
            activeUsers = new ArrayList<>();
            File file = new File("src/files/");
            file.mkdirs();
            File file2 = new File("src/downloads/");
            file2.mkdirs();
            File file3 = new File("src/others/");
            file3.mkdirs();

            while (true){
                System.out.println("Waiting for connection...");
                Socket clientSocket = serverSocket.accept();
                System.out.println("Connection established");
                serve(clientSocket);
            }
        }catch (Exception e) {
            System.out.println("Server starts:" + e);
        }
    }
}

