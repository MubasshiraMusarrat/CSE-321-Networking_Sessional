import java.io.*;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Objects;

public class Worker extends Thread{
    private final Server server ;
    private final Socket socket;
    private String username;
    private int requestCount=0;
    private int fileCount=0;
    private ObjectOutputStream out;
    private ObjectInputStream in;

    private void downloadFiles(File downloadFile) throws IOException {
        int chunkSize = server.getMaxChunkSize();
        out.writeObject(chunkSize);
        long filesize = downloadFile.length();
        out.writeObject(filesize);
        FileInputStream input = new FileInputStream(downloadFile);
        byte[] buffer = new byte[chunkSize];
        int bytes;
        while ((bytes = input.read(buffer))!=-1){
            out.write(buffer,0,bytes);
            out.flush();
        }
        out.writeObject("Download completed");
        input.close();
    }

    private void uploadFiles(String fileType, boolean flag, String requestID) throws IOException, ClassNotFoundException {
        out.writeObject("Enter file name:");
        String fileName = (String) in.readObject();
        out.writeObject("Enter file size:");
        long fileSize = (long) in.readObject();
        if (server.getMaxBufferSize() < server.getUsedBuffer() + fileSize) {
            out.writeObject("Buffer Overflowed.Transmission denied");
        }
        else {
            int maxValue = server.getMaxChunkSize();
            int minValue = server.getMinChunkSize();
            int chunkSize = (int) ((Math.random()*(maxValue-minValue+1))+minValue);
            fileCount++;
            String fileID = username + "_" + fileCount;
            server.getFileMap().put(fileID,fileName);
            out.writeObject("Chunk size:");
            out.writeObject(chunkSize);
            out.writeObject("File ID:");
            out.writeObject(fileID);
            FileOutputStream output = new FileOutputStream("src/files/" + username + "/" + fileType + "/" + fileName);
            int bytes;
            byte[] buffer = new byte[chunkSize];
            int chunkCount = 0;
            long tempFileSize = fileSize;
            long bytes_sum = 0;
            try {
                while (fileSize > 0 && (bytes = in.read(buffer, 0, (int) Math.min(buffer.length, fileSize))) != -1) {
                    output.write(buffer, 0, bytes);
                    //System.out.println(fileSize);
                    fileSize -= bytes;
                    //System.out.println(bytes_sum);
                    server.UseBuffer(bytes);
                    bytes_sum += bytes;
                    chunkCount++;
                    //Thread.sleep(10000);
                    if (fileSize == 0) {
                        out.writeObject("last chunk");
                    } else {
                        out.writeObject("chunk number: "+chunkCount);
                    }
                    String acknowledgement = (String) in.readObject();
                    //System.out.println(acknowledgement);
                    if (acknowledgement.equalsIgnoreCase("timeout")) {
                        File file = new File("src/files/" + username + "/" + fileType + "/" + fileName);
                        file.delete();
                    }
                }
                String completeMsg = (String) in.readObject();
                //System.out.println(completeMsg);
                if (completeMsg.equalsIgnoreCase("file upload completed")) {
                    System.out.println(completeMsg);
                    //System.out.println(tempFileSize);
                   // System.out.println(bytes_sum);
                    if (tempFileSize == bytes_sum) {
                        out.writeObject("Uploaded successfully");
                        Server.UseBuffer((int) -tempFileSize);
                        //System.out.println(server.getUsedBuffer());
                        if(flag){
                            ArrayList<String> message = server.getClientInbox().get(requestID);
                            message.add("File uploaded:\nUsername: "+username+"\nFilename: "+fileName);
                            server.getClientInbox().replace(requestID,message);
                        }
                    } else {
                        out.writeObject("Uploading failed");
                    }
                }
                output.close();
            } catch (SocketException e){
                System.out.println("Sender got disconnected");
                File file = new File("src/files/" + username + "/" + fileType + "/" + fileName);
                file.delete();

            }
        }
    }

    public Worker( Socket socket){
        server = new Server();
        this.socket = socket;
    }

    @Override
    public void run(){
        try{
            out = new ObjectOutputStream(this.socket.getOutputStream());
            in = new ObjectInputStream(this.socket.getInputStream());

            out.writeObject("Enter Username: ");
            username = (String) in.readObject();

            boolean flag = true;
            //System.out.println(Integer.parseInt(String.valueOf(server.getActiveUsers().size())));
                for (int i = 0; i < Server.getActiveUsers().size(); i++) {
                    //System.out.println("Dhukse");
                    if (Server.getActiveUsers().get(i).equals(username)) {
                        flag = false;
                        Server.getActiveUsers().remove(username);
                        out.writeObject(false);
                        socket.close();
                        return;
                    }
                }

            //System.out.println(Boolean.toString(flag));
            if(flag){
                out.writeObject(true);
                Server.getActiveUsers().add(username);
                //System.out.println(server.getActiveUsers().get(0));
                boolean connected = true;
                //System.out.println(Integer.parseInt(String.valueOf(server.getConnectedUsers().size())));
                for(int i = 0; i< Server.getConnectedUsers().size(); i++){
                    if(Server.getConnectedUsers().get(i).equals(username)) {
                        connected = false;
                        break;
                    }
                }
                //System.out.println(Boolean.toString(connected));
                if(connected){
                    Server.getConnectedUsers().add(username);
                    File PublicFiles = new File("src/files/"+username+"/public");
                    PublicFiles.mkdirs();
                    File PrivateFiles = new File("src/files/"+username+"/private");
                    PrivateFiles.mkdirs();
                    ArrayList<String> inbox = new ArrayList<>();
                    server.getClientInbox().put(username,inbox);
                }
            }

            while(true) {
                String command = (String) in.readObject();

                //log out
                if (command.equalsIgnoreCase("1")) {
                    System.out.println(username + " logging out");
                    Server.getActiveUsers().remove(username);
                    out.writeObject("Logging out");
                    socket.close();
                    return;
                }

                //look for other clients
                else if (command.equalsIgnoreCase("2")) {
                    out.writeObject("Sending list");
                    int count = Server.getConnectedUsers().size();
                    out.writeObject(count - 1);
                    for (int i = 0; i < count; i++) {
                        String connectedUser = Server.getConnectedUsers().get(i);
                        if (!(connectedUser.equals(username))) {
                            int activeCount = Server.getActiveUsers().size();
                            boolean active = false;
                            for (int j = 0; j < activeCount; j++) {
                                if (Server.getActiveUsers().get(j).equals(connectedUser)) {
                                    String activeUser = connectedUser + " (Online)";
                                    active = true;
                                    System.out.println(activeUser);
                                    out.writeObject(activeUser);
                                }
                            }
                            if (!active) {
                                System.out.println(connectedUser);
                                out.writeObject(connectedUser);
                            }
                        }
                    }
                }

                // look for own private and public file
                else if (command.equalsIgnoreCase("3")) {
                    out.writeObject("sending own file list");
                    out.writeObject("Public");
                    File Public = new File("src/files/" + username + "/public");
                    String[] contentsPublic = Public.list();
                    assert contentsPublic != null;
                    int PublicSize = contentsPublic.length;
                    out.writeObject(PublicSize);
                    for (String s : contentsPublic) {
                        out.writeObject(s);
                    }

                    out.writeObject("Private");
                    File Private = new File("src/files/" + username + "/private");
                    String[] contentsPrivate = Private.list();
                    assert contentsPrivate != null;
                    int PrivateSize = contentsPrivate.length;
                    out.writeObject(PrivateSize);
                    for (String s : contentsPrivate) {
                        out.writeObject(s);
                    }
                }

                //download own files
                else if (command.equalsIgnoreCase("4")) {
                    out.writeObject("Public or Private?");
                    String fileType = (String) in.readObject();
                    if (fileType.equalsIgnoreCase("public")) {
                        fileType = "public";
                    } else {
                        fileType = "private";
                    }
                    out.writeObject("Enter filename:");
                    String fileName = (String) in.readObject();
                    File Search = new File("src/files/" + username + "/" + fileType);
                    String[] contents = Search.list();
                    assert contents != null;
                    for (String content : contents) {
                        if (content.contains(fileName)) {
                            fileName = content;
                            break;
                        }
                    }
                    out.writeObject(fileName);
                    File downloadFile = new File("src/files/" + username + "/" + fileType + "/" + fileName);
                    downloadFiles(downloadFile);
                }

                //look for other public files
                else if (command.equalsIgnoreCase("5")) {
                    out.writeObject("sending others file list");
                    File source = new File("src/files");
                    if (source.isDirectory()) {
                        int userCount = Objects.requireNonNull(source.listFiles()).length;
                        out.writeObject(userCount - 1);
                        //System.out.println(userCount);
                        if (userCount > 0) {
                            File[] subFolders = source.listFiles();
                            for (int i = 0; i < userCount; i++) {
                                assert subFolders != null;
                                if (!(subFolders[i].getName().equals(username))) {
                                    out.writeObject(subFolders[i].getName());
                                    File other = new File("src/files/" + subFolders[i].getName() + "/public");
                                    String[] OtherContents = other.list();
                                    assert OtherContents != null;
                                    int otherFileCount = OtherContents.length;
                                    //System.out.println(otherFileCount);
                                    out.writeObject(otherFileCount);
                                    for (String otherContent : OtherContents) {
                                        out.writeObject(otherContent);
                                    }
                                }
                            }
                        }
                    }
                }

                //download others' files
                else if (command.equalsIgnoreCase("6")) {
                    out.writeObject("send download details");
                    out.writeObject("Enter username:");
                    String fileUsername = (String) in.readObject();
                    File Search = new File("src/files");
                    String[] fileUsers = Search.list();
                    assert fileUsers != null;
                    for (String fileUser : fileUsers) {
                        if (fileUser.contains(fileUsername)) {
                            fileUsername = fileUser;
                            break;
                        }
                    }
                    out.writeObject("Enter file name:");
                    String fileName = (String) in.readObject();
                    File Search2 = new File("src/files/" + fileUsername + "/public");
                    String[] OtherFiles = Search2.list();
                    assert OtherFiles != null;
                    for (String otherFile : OtherFiles) {
                        if (otherFile.contains(fileName)) {
                            fileName = otherFile;
                            break;
                        }
                    }
                    out.writeObject(fileName);
                    File downloadFile = new File("src/files/" + fileUsername + "/public/" + fileName);
                    downloadFiles(downloadFile);
                }

                //request for a file
                else if (command.equalsIgnoreCase("7")) {
                    out.writeObject("Enter a short description of the file:");
                    String description = (String) in.readObject();
                    requestCount++;
                    String requestID = username + "_" + requestCount;
                    server.getFileRequest().put(requestID, description);
                    if (Server.getConnectedUsers().size() > 1) {
                        for (int i = 0; i < Server.getConnectedUsers().size(); i++) {
                            String UserInbox = Server.getConnectedUsers().get(i);
                            if (!(UserInbox.equals(username))) {
                                ArrayList<String> message = server.getClientInbox().get(UserInbox);
                                message.add("File requested:\nRequest ID: " + requestID + "\n\"" + description + "\"");
                                server.getClientInbox().replace(UserInbox, message);
                                //System.out.println(server.getClientInbox().get(UserInbox));
                            }
                        }
                        out.writeObject("Request sent");
                    } else {
                        System.out.println("No other user exists");
                    }
                }

                //view unread messages
                else if (command.equalsIgnoreCase("8")) {
                    out.writeObject("Unread messages");
                    ArrayList<String> inbox = server.getClientInbox().get(username);
                    out.writeObject(inbox.size());
                    if (inbox.size() > 0) {
                        for (String s : inbox) {
                            out.writeObject(s);
                        }
                        server.getClientInbox().replace(username, new ArrayList<>());
                    }
                }

                //upload files
                else if (command.equalsIgnoreCase("9")) {
                    out.writeObject("Uploading started");
                    out.writeObject("Public or Private?");
                    String fileType = (String) in.readObject();
                    if(fileType.equalsIgnoreCase("public")){
                        fileType = "public";
                    }
                    else{
                        fileType = "private";
                    }
                    uploadFiles(fileType,false,null);

                }

                //uploading file on the basis of a request
                else if(command.equalsIgnoreCase("10")){
                    out.writeObject("Enter request id:");
                    String requestID = (String) in.readObject();
                    String requestUser = requestID.split("_")[0];
                    System.out.println(requestUser);
                    String fileType = "public";
                    uploadFiles(fileType,true,requestUser);
                }
                else {
                    System.out.println("Unrecognised command");
                }
            }
                //System.out.println("Shesh");
        } catch (IOException | ClassNotFoundException e){
            throw new RuntimeException(e);
        }
    }
}
