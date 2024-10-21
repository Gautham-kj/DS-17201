import java.io.*;
import java.net.*;

public class Client {
    public static void main(String[] args) {
        final String SERVER_ADDRESS = "127.0.0.1";
        final int SERVER_PORT = 8080;

        try (Socket socket = new Socket(SERVER_ADDRESS, SERVER_PORT)) {
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));

            System.out.println("Connected to server. Type messages and press Enter to send.");

            String userInput;
            while ((userInput = stdIn.readLine()) != null) {
                out.println(userInput);
                String response = in.readLine();
                System.out.println("Server: " + response);
            }
        } catch (UnknownHostException e) {
            System.err.println("Unknown host: " + SERVER_ADDRESS);
            e.printStackTrace();
        } catch (IOException e) {
            System.err.println("I/O error when connecting to server");
            e.printStackTrace();
        }
    }
}