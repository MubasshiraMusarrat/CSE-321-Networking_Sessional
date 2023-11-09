import java.io.*;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.util.Scanner;

public class Client {
    private static Socket socket;
    private static ObjectInputStream in;
    private static ObjectOutputStream out;
    private static void downloadFile(FileOutputStream output) throws IOException, ClassNotFoundException {
        int chunkSize = (int) in.readObject();
        byte[] buffer = new byte[chunkSize];
        long fileSize = (long) in.readObject();
        int bytes;
        while (fileSize > 0 && ( bytes = in.read(buffer, 0, (int)Math.min(buffer.length, fileSize))) != -1) {
            output.write(buffer,0,bytes);
            fileSize -= bytes;
        }
        System.out.println((String) in.readObject());
        output.close();
    }

    private static void uploadFile() throws IOException, ClassNotFoundException {

        System.out.println(in.readObject());
        Scanner scanner = new Scanner(System.in);
        String fileName = scanner.nextLine();
        out.writeObject(fileName);
        System.out.println(in.readObject());
        File file = new File("src/others/" + fileName);
        long fileSize = file.length();
        System.out.println(fileSize);
        out.writeObject(fileSize);
        String msg = (String) in.readObject();
        if(msg.equalsIgnoreCase("Buffer Overflowed.Transmission denied")){
            System.out.println(msg);
        }
        else if(msg.equalsIgnoreCase("Chunk size:")){
            int chunkSize = (int) in.readObject();
            System.out.println(msg+" "+chunkSize);
            String fileMsg = (String) in.readObject();
            if(fileMsg.equalsIgnoreCase("File ID:")){
                String fileID = (String) in.readObject();
                System.out.println(fileMsg+" "+fileID);
                FileInputStream input = new FileInputStream(file);
                int bytes;
                byte[] buffer = new byte[chunkSize];
                socket.setSoTimeout(30000);
                try{
                    while ((bytes=input.read(buffer))!=-1){
                        out.write(buffer,0,bytes);
                        out.flush();
                        String chunkMsg = (String) in.readObject();
                        out.writeObject("Acknowledged");
                        //System.out.println(chunkMsg);
                        if(chunkMsg.equalsIgnoreCase("last chunk")){
                            //System.out.println("Dhuktese");
                            out.writeObject("file upload completed");
                            System.out.println(in.readObject());
                        }

                    }
                    input.close();
                }
                catch (SocketTimeoutException e){
                    out.writeObject("timeout");
                    System.out.println("Uploading failed");
                }
            }
        }
    }

    public static void main(String[] args) {
        String serverAddress = "localhost";
        int serverPort = 6666;
        Scanner scanner = new Scanner(System.in);

        try {
            socket = new Socket(serverAddress, serverPort);
            System.out.println("Connection established");

            out = new ObjectOutputStream(socket.getOutputStream());
            in = new ObjectInputStream(socket.getInputStream());

            String usernameMsg = (String) in.readObject();
            System.out.println(usernameMsg);
            String ID = scanner.nextLine();
            out.writeObject(ID);

        // log In
        Boolean login = (Boolean) in.readObject();
        if(!login){
            System.out.println("Already logged in from different IP");
            socket.close();
            System.exit(0);
        }
        else{
            System.out.println("Login Successful");
        }

        while(true){
            System.out.println("1.Log out");
            System.out.println("2. Look for other clients");
            System.out.println("3. Look for own files");
            System.out.println("4. Download own file");
            System.out.println("5. Look for other files");
            System.out.println("6. Download others' file");
            System.out.println("7. Request for a file");
            System.out.println("8. View unread messages");
            System.out.println("9. Upload a file");
            System.out.println("10. Upload a file (on the basis of a request)");
            String command = scanner.nextLine();
            out.writeObject(command);
            String fromServer = (String) in.readObject();

            if(fromServer.equalsIgnoreCase("Logging out")){
                System.out.println("Logging out");
                socket.close();
                System.exit(0);
            }

            else if(fromServer.equalsIgnoreCase("Sending List")){
                int count = (int)in.readObject();
                for(int i=0; i<count; i++){
                    String user = (String)in.readObject();
                    System.out.println(user);
                }
            }

            else if(fromServer.equalsIgnoreCase("sending own file list")){
                for(int k=0; k<2 ; k++) {
                    String fileType = (String) in.readObject();
                    System.out.println(fileType+": ");
                    int fileCount = (int) in.readObject();
                    if(fileCount>0) {
                        for (int i = 0; i < fileCount; i++) {
                            String content = (String) in.readObject();
                            System.out.println(content);
                        }
                    }
                    else{
                        System.out.println("No files in the "+fileType+" directory");
                    }
                }
            }

            else if(fromServer.equalsIgnoreCase("Public or Private?")){
                System.out.println(fromServer);
                String fileType = scanner.nextLine();
                out.writeObject(fileType);
                System.out.println(in.readObject());
                String fileName = scanner.nextLine();
                out.writeObject(fileName);
                fileName = (String) in.readObject();
                FileOutputStream output = new FileOutputStream("src/downloads/" + fileName);
                downloadFile(output);

            }

            else if(fromServer.equalsIgnoreCase("sending others file list")){
                int userCount = (int)in.readObject();
                //System.out.println(userCount);
                if(userCount == 0){
                    System.out.println("No other user exists");
                }
                else{
                    System.out.println("All files shown are inside other user's public directory");
                    for(int i=0;i<userCount;i++){
                        String otherUserName = (String) in.readObject();
                        System.out.println(otherUserName+": ");
                        int otherUserFileCount = (int) in.readObject();
                        //System.out.println(otherUserFileCount);
                        if(otherUserFileCount == 0){
                            System.out.println("No files exist for this user");
                        }
                        else{
                            for(int j=0; j<otherUserFileCount; j++){
                                String otherFileName = (String) in.readObject();
                                System.out.println(otherFileName);
                            }
                        }
                    }
                }
            }

            else if(fromServer.equalsIgnoreCase("send download details")) {
                System.out.println(in.readObject());
                String fileUsername = scanner.nextLine();
                out.writeObject(fileUsername);
                System.out.println(in.readObject());
                String fileName = scanner.nextLine();
                out.writeObject(fileName);
                fileName = (String) in.readObject();
                FileOutputStream output = new FileOutputStream("src/downloads/" + fileName);
                downloadFile(output);
            }

            else if(fromServer.equalsIgnoreCase("Enter a short description of the file:")){
                System.out.println(fromServer);
                String description = scanner.nextLine();
                out.writeObject(description);
                System.out.println(in.readObject());
            }

            else if(fromServer.equalsIgnoreCase("Unread messages")){
                System.out.println(fromServer);
                int inboxSize = (int) in.readObject();
                if(inboxSize>0){
                    for(int i=0;i<inboxSize;i++){
                        System.out.println((i+1)+") "+in.readObject());
                    }
                }
                else{
                    System.out.println("No unread messages");
                }
            }

            else if(fromServer.equalsIgnoreCase("Uploading started")){
                System.out.println(in.readObject());
                String fileType = scanner.nextLine();
                out.writeObject(fileType);
                uploadFile();
            }

            else if(fromServer.equalsIgnoreCase("Enter request id:")){
                System.out.println(fromServer);
                String requestID = scanner.nextLine();
                out.writeObject(requestID);
                uploadFile();
            }

            //System.out.println("Shesh");
            }
        } catch (IOException | ClassNotFoundException e) {
            throw new RuntimeException(e);
        }
    }
}
