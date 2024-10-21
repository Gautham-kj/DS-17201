import java.io.*;
import java.net.*;

public class Server {
    public static void main(String[] args) {
        final int PORT = 8080;

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Server listening on port " + PORT);

            while (true) {
                try (Socket clientSocket = serverSocket.accept()) {
                    System.out.println("Accepted connection from " + clientSocket.getInetAddress() + ":" + clientSocket.getPort());

                    BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                    PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

                    String inputLine;
                    while ((inputLine = in.readLine()) != null) {
                        String upperCaseString = inputLine.toUpperCase();
                        out.println(upperCaseString);
                    }
                } catch (IOException e) {
                    System.err.println("Error handling client connection: " + e.getMessage());
                }
            }
        } catch (IOException e) {
            System.err.println("Could not listen on port " + PORT);
            e.printStackTrace();
        }
    }
}