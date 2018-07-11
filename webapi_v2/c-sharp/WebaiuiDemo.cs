using System;
using System.Text;
using System.Linq;
using System.Net;
using System.Web;
using System.IO;
using System.Collections.Generic;
using System.Security.Cryptography;

/**
 * AIUI WebAPI V2接口调用示例
 * 
 * 运行方法：直接运行 main()
 * 
 * 结果： 控制台输出接口返回值信息
 * 
 * @author iflytek_aiui
 * 
 */
class WebaiuiDemo{
	private const String URL = "http://openapi.xfyun.cn/v2/aiui";
	private const String APPID = "";
	private const String API_KEY = "";
	private const String AUE = "raw";
	private const String AUTH_ID = "2894c985bf8b1111c6728db79d3479ae";
	private const String DATA_TYPE = "audio";
	private const String SAMPLE_RATE = "16000";
	private const String SCENE = "main";
	private const String RESULT_LEVEL = "complete";
	private const String LAT = "39.938838";
	private const String LNG = "116.368624";
	private const String FILE_PATH = "";
	// 个性化参数，需转义
	private const String PERS_PARAM = "{\\\"auth_id\\\":\\\"2894c985bf8b1111c6728db79d3479ae\\\"}";
	
	public static void Main(){
        Dictionary<String,String> header = buildHeader();
		byte[] dataByteArray = readFile(FILE_PATH);
		String result = httpPost(URL,header,dataByteArray);
		Console.WriteLine(result); 
		Console.ReadLine(); 
    }
	
	private static Dictionary<String,String> buildHeader(){
		String curTime = DateTime.Now.Second.ToString();
		String param = "{\"aue\":\""+AUE+"\",\"result_level\":\""+RESULT_LEVEL+"\",\"sample_rate\":\""+SAMPLE_RATE+"\",\"auth_id\":\""+AUTH_ID+"\",\"data_type\":\""+DATA_TYPE+"\",\"scene\":\""+SCENE+"\"}";
		//使用个性化参数时参数格式如下：
		//String param = "{\"aue\":\""+AUE+"\",\"sample_rate\":\""+SAMPLE_RATE+"\",\"auth_id\":\""+AUTH_ID+"\",\"data_type\":\""+DATA_TYPE+"\",\"scene\":\""+SCENE+"\",\"pers_param\":\""+PERS_PARAM+"\"}";
		String paramBase64 = System.Convert.ToBase64String(System.Text.Encoding.Default.GetBytes(param));
		String checkSum = EncryptWithMD5(API_KEY + curTime + paramBase64);
		
		Dictionary<String, String> header = new Dictionary<String,String>();
		header.Add("X-Param",paramBase64);
		header.Add("X-CurTime",curTime);
		header.Add("X-CheckSum",checkSum);
		header.Add("X-Appid",APPID);
		return header;
	}
	
	private static String EncryptWithMD5(String source){
		byte[] sor = Encoding.UTF8.GetBytes(source);
		MD5 md5 = MD5.Create();
		byte[] result = md5.ComputeHash(sor);
		StringBuilder strbul = new StringBuilder(40);
		for (int i = 0; i < result.Length; i++){
			//加密结果"x2"结果为32位,"x3"结果为48位,"x4"结果为64位
			strbul.Append(result[i].ToString("x2"));
		}
		return strbul.ToString();
	}
	
	private static byte[] readFile(String filePath){
		FileStream fs = new FileStream(filePath,FileMode.Open);
		byte[] data = new byte[fs.Length];
		fs.Read(data, 0, data.Length);
		fs.Close();
		return data;
	}
	
	private static String httpPost(String url, Dictionary<String, String> headerDic, byte[] body){
		HttpWebRequest httpWebRequest = null;
		HttpWebResponse httpWebResponse = null;
		String result = "";
		try{               
			httpWebRequest = (HttpWebRequest)HttpWebRequest.Create(url);
			httpWebRequest.Method = "POST";
			
			httpWebRequest.Headers.Add("X-Param",headerDic["X-Param"]);
			httpWebRequest.Headers.Add("X-CurTime",headerDic["X-CurTime"]);
			httpWebRequest.Headers.Add("X-CheckSum",headerDic["X-CheckSum"]);
			httpWebRequest.Headers.Add("X-Appid",headerDic["X-Appid"]);

			httpWebRequest.ContentLength = body.Length;
			using (Stream stream = httpWebRequest.GetRequestStream())
			{
				stream.Write(body, 0, body.Length);
			}
			httpWebResponse = (HttpWebResponse)httpWebRequest.GetResponse();
			Stream responseStream = httpWebResponse.GetResponseStream();			
			result = new StreamReader(responseStream, Encoding.GetEncoding("utf-8")).ReadToEnd();		

			responseStream.Close();
			httpWebRequest.Abort();
			httpWebResponse.Close();
		}
		catch (Exception ex){
			 Console.Write("Error: " + ex.ToString());
		}
		return result;
	} 
}