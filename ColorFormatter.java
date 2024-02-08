import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * ColorFormatter
 */
public class ColorFormatter {

    private File colorMapFile;
    private Map<String, Integer[]> colorMap;

    public ColorFormatter(String filepath) {
        colorMapFile = new File(filepath);
        colorMap = new HashMap<>();
        retrieveData();
    }

    private void retrieveData() {
        try (BufferedReader br = new BufferedReader(new FileReader(colorMapFile))) {
            String line;
            while ((line = br.readLine()) != null) {
                System.out.println("Line content: " + line); // Debug statement
                String[] parts = line.split("\t");
                printLine(parts);
                System.out.println("\nParts length: " + parts.length); // Debug statement
                if (parts.length >= 4) { // Ensure that the line has at least four parts
                    String colorName = parts[1]; // Replace underscores with spaces
                    Integer[] rgbValues = new Integer[3];
                    rgbValues[0] = Integer.parseInt(parts[3]);
                    rgbValues[1] = Integer.parseInt(parts[4]);
                    rgbValues[2] = Integer.parseInt(parts[5]);
                    colorMap.put(colorName, rgbValues);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void printLine(String[] arr) {
        for (String string : arr) {
            System.out.print(string + ", ");
        }
    }

    public void writeToCSV(String outputPath) {
        try (FileWriter writer = new FileWriter(outputPath)) {
            for (Map.Entry<String, Integer[]> entry : colorMap.entrySet()) {
                StringBuilder sb = new StringBuilder();
                for (Integer value : entry.getValue()) {
                    sb.append(value).append(", ");
                }
                sb.append(entry.getKey()).append("\n");
                writer.write(sb.toString());
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        String inputFilePath = "rawColorFile.txt";
        String outputFilePath = "colormap.csv";
        ColorFormatter formatter = new ColorFormatter(inputFilePath);
        formatter.writeToCSV(outputFilePath);
    }
}