package com.iflytek.aiui.demo.chat;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Base64;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;

import org.json.JSONException;
import org.json.JSONObject;

import com.iflytek.aiui.AIUIAgent;
import com.iflytek.aiui.AIUIConstant;
import com.iflytek.aiui.AIUIEvent;
import com.iflytek.aiui.AIUIListener;
import com.iflytek.aiui.AIUIMessage;
import com.iflytek.aiui.demo.chat.util.FucUtil;
//import com.iflytek.speech.util.FucUtil;

/**
 * 语义理解demo。
 */
public class NlpDemo extends Activity implements OnClickListener {
	private static String TAG = NlpDemo.class.getSimpleName();

	private Toast mToast;
	private TextView mTimeSpentText;
	private EditText mNlpText;

	private AIUIAgent mAIUIAgent = null;
	private int mAIUIState = AIUIConstant.STATE_IDLE;

	private String mSyncSid = "";
	
	@SuppressLint("ShowToast")
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.nlpdemo);

		initLayout();

		mToast = Toast.makeText(this, "", Toast.LENGTH_SHORT);
	}
	
	private void initLayout() {
		findViewById(R.id.nlp_create).setOnClickListener(NlpDemo.this);
		findViewById(R.id.nlp_destroy).setOnClickListener(NlpDemo.this);
		findViewById(R.id.nlp_start).setOnClickListener(NlpDemo.this);
		findViewById(R.id.nlp_stop_rec).setOnClickListener(NlpDemo.this);
		findViewById(R.id.text_nlp_start).setOnClickListener(NlpDemo.this);
		findViewById(R.id.sync_contacts).setOnClickListener(NlpDemo.this);
		findViewById(R.id.sync_query).setOnClickListener(NlpDemo.this);

		mTimeSpentText = (TextView) findViewById(R.id.txt_time_spent);
		mNlpText = (EditText) findViewById(R.id.nlp_text);
	}

	@Override
	public void onClick(View view) {
		int[] pos = new int[2];
		view.getLocationOnScreen(pos);

		int x = pos[0] + view.getWidth() / 2;
		int y = pos[1] + view.getHeight() / 2;

		Log.d("button_pos", "x:" + x + ", y:" + y);

		switch (view.getId()) {
			// 创建AIUIAgent
			case R.id.nlp_create:
				createAgent();
				break;
			// 销毁AIUIAgent
			case R.id.nlp_destroy:
				destroyAgent();
				break;
			// 开始文本语义
			case R.id.text_nlp_start:
				startTextNlp();
				break;
			// 开始语音语义
			case R.id.nlp_start:
				startVoiceNlp();
				break;
			// 停止语音语义
			case R.id.nlp_stop_rec:
				stopVoiceNlp();
				break;
			// 同步联系人
			case R.id.sync_contacts:
				syncContacts();
				break;
			// 联系人资源打包状态查询
			case R.id.sync_query:
				syncQuery();
				break;

			default:
				break;
		}
	}
	
	private String getAIUIParams() {
		String params = "";

		AssetManager assetManager = getResources().getAssets();
		try {
			InputStream ins = assetManager.open( "cfg/aiui_phone.cfg" );
			byte[] buffer = new byte[ins.available()];

			ins.read(buffer);
			ins.close();

			params = new String(buffer);

			JSONObject paramsJson = new JSONObject(params);

			params = paramsJson.toString();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (JSONException e) {
			e.printStackTrace();
		}

		return params;
	}

	private void createAgent() {
		if (null == mAIUIAgent) {
			Log.i(TAG, "create aiui agent");

			mAIUIAgent = AIUIAgent.createAgent(this, getAIUIParams(), mAIUIListener);
		}

		if (null == mAIUIAgent) {
			final String strErrorTip = "创建AIUIAgent失败！";
			showTip(strErrorTip);

			mNlpText.setText(strErrorTip);
		} else {
			showTip("AIUIAgent已创建");
		}
	}

	private void destroyAgent() {
		if (null != mAIUIAgent) {
			Log.i(TAG, "destroy aiui agent");

			mAIUIAgent.destroy();
			mAIUIAgent = null;

			showTip("AIUIAgent已销毁");
		} else {
			showTip("AIUIAgent为空");
		}
	}
	
	private void startVoiceNlp(){
		if (null == mAIUIAgent) {
			showTip("AIUIAgent为空，请先创建");
			return;
		}

		Log.i( TAG, "start voice nlp" );
		mNlpText.setText("");
		
		// 先发送唤醒消息，改变AIUI内部状态，只有唤醒状态才能接收语音输入
		// 默认为oneshot模式，即一次唤醒后就进入休眠。可以修改aiui_phone.cfg中speech参数的interact_mode为continuous以支持持续交互
//		if (AIUIConstant.STATE_WORKING != mAIUIState)
		{
			AIUIMessage wakeupMsg = new AIUIMessage(AIUIConstant.CMD_WAKEUP, 0, 0, "", null);
			mAIUIAgent.sendMessage(wakeupMsg);
		}
		
		// 打开AIUI内部录音机，开始录音。若要使用上传的个性化资源增强识别效果，则在参数中添加pers_param设置
		// 个性化资源使用方法可参见http://doc.xfyun.cn/aiui_mobile/的用户个性化章节
		// 在输入参数中设置tag，则对应结果中也将携带该tag，可用于关联输入输出
		String params = "sample_rate=16000,data_type=audio,pers_param={\"uid\":\"\"},tag=audio-tag";
		AIUIMessage startRecord = new AIUIMessage(AIUIConstant.CMD_START_RECORD, 0, 0, params, null);

		mAIUIAgent.sendMessage(startRecord);
	}
	
	private void stopVoiceNlp(){
		if (null == mAIUIAgent) {
			showTip("AIUIAgent 为空，请先创建");
			return;
		}

		Log.i( TAG, "stop voice nlp" );
		// 停止录音
		String params = "sample_rate=16000,data_type=audio";
		AIUIMessage stopRecord = new AIUIMessage(AIUIConstant.CMD_STOP_RECORD, 0, 0, params, null);

		mAIUIAgent.sendMessage(stopRecord);
	}
	
	private void startTextNlp() {
		if (null == mAIUIAgent) {
			showTip("AIUIAgent 为空，请先创建");
			return;
		}

		// 先发送唤醒消息，改变AIUI内部状态，只有唤醒状态才能接收文本输入
//		if (AIUIConstant.STATE_WORKING != mAIUIState)
		{
			AIUIMessage wakeupMsg = new AIUIMessage(AIUIConstant.CMD_WAKEUP, 0, 0, "", null);
			mAIUIAgent.sendMessage(wakeupMsg);
		}

		Log.i( TAG, "start text nlp" );
		String text = "合肥明天的天气怎么样？";
		mNlpText.setText(text);

		try {
			// 在输入参数中设置tag，则对应结果中也将携带该tag，可用于关联输入输出
			String params = "data_type=text,tag=text-tag";
			byte[] textData = text.getBytes("utf-8");

			AIUIMessage write = new AIUIMessage(AIUIConstant.CMD_WRITE, 0, 0, params, textData);
			mAIUIAgent.sendMessage(write);

		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
	}
	
	private void syncContacts() {
		if (null == mAIUIAgent) {
			showTip("AIUIAgent 为空，请先创建");
			return;
		}


		try {
			// 从文件中读取联系人示例数据
			String dataStr = FucUtil.readFile(this, "data_contact.txt","utf-8");
			mNlpText.setText(dataStr);

			// 数据进行no_wrap Base64编码
			String dataStrBase64 = Base64.encodeToString(dataStr.getBytes("utf-8"), Base64.NO_WRAP);

			JSONObject syncSchemaJson = new JSONObject();
			JSONObject dataParamJson = new JSONObject();

			// 设置id_name为uid，即用户级个性化资源
			// 个性化资源使用方法可参见http://doc.xfyun.cn/aiui_mobile/的用户个性化章节
			dataParamJson.put("id_name", "uid");

			// 设置res_name为联系人
			dataParamJson.put("res_name", "IFLYTEK.telephone_contact");

			syncSchemaJson.put("param", dataParamJson);
			syncSchemaJson.put("data", dataStrBase64);

			// 传入的数据一定要为utf-8编码
			byte[] syncData = syncSchemaJson.toString().getBytes("utf-8");

			// 给该次同步加上自定义tag，在返回结果中可通过tag将结果和调用对应起来
			JSONObject paramJson = new JSONObject();
			paramJson.put("tag", "sync-tag");

			// 用schema数据同步上传联系人
			// 注：数据同步请在连接服务器之后进行，否则可能失败
			AIUIMessage syncAthena = new AIUIMessage(AIUIConstant.CMD_SYNC,
					AIUIConstant.SYNC_DATA_SCHEMA, 0, paramJson.toString(), syncData);

			mAIUIAgent.sendMessage(syncAthena);
		} catch (JSONException e) {
			e.printStackTrace();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
	}

	private void syncQuery() {
		if (null == mAIUIAgent) {
			showTip("AIUIAgent 为空，请先创建");
			return;
		}


		if (TextUtils.isEmpty(mSyncSid)) {
			showTip("sid 为空");
			return;
		}

		try {
			// 构造查询json字符串，填入同步schema数据返回的sid
			JSONObject queryJson = new JSONObject();
			queryJson.put("sid", mSyncSid);

			// 发送同步数据状态查询消息，设置arg1为schema数据类型，params为查询字符串
			AIUIMessage syncQuery = new AIUIMessage(AIUIConstant.CMD_QUERY_SYNC_STATUS,
					AIUIConstant.SYNC_DATA_SCHEMA, 0, queryJson.toString(), null);
			mAIUIAgent.sendMessage(syncQuery);
		} catch (JSONException e) {
			e.printStackTrace();
		}
	}
	
	private AIUIListener mAIUIListener = new AIUIListener() {

		@Override
		public void onEvent(AIUIEvent event) {
			Log.i( TAG,  "on event: " + event.eventType );

			switch (event.eventType) {
				case AIUIConstant.EVENT_CONNECTED_TO_SERVER:
					showTip("已连接服务器");
					break;

				case AIUIConstant.EVENT_SERVER_DISCONNECTED:
					showTip("与服务器断连");
					break;

				case AIUIConstant.EVENT_WAKEUP: 
					showTip( "进入识别状态" );
					break;
	
				case AIUIConstant.EVENT_RESULT: {
					try {
						JSONObject bizParamJson = new JSONObject(event.info);
						JSONObject data = bizParamJson.getJSONArray("data").getJSONObject(0);
						JSONObject params = data.getJSONObject("params");
						JSONObject content = data.getJSONArray("content").getJSONObject(0);

						if (content.has("cnt_id")) {
							String cnt_id = content.getString("cnt_id");
							String cntStr = new String(event.data.getByteArray(cnt_id), "utf-8");

							// 获取该路会话的id，将其提供给支持人员，有助于问题排查
							// 也可以从Json结果中看到
							String sid = event.data.getString("sid");
							String tag = event.data.getString("tag");

							showTip("tag=" + tag);

							// 获取从数据发送完到获取结果的耗时，单位：ms
							// 也可以通过键名"bos_rslt"获取从开始发送数据到获取结果的耗时
							long eosRsltTime = event.data.getLong("eos_rslt", -1);
							mTimeSpentText.setText(eosRsltTime + "ms");

							if (TextUtils.isEmpty(cntStr)) {
								return;
							}

							JSONObject cntJson = new JSONObject(cntStr);

							if (mNlpText.getLineCount() > 1000) {
								mNlpText.setText("");
							}

							mNlpText.append( "\n" );
							mNlpText.append(cntJson.toString());
							mNlpText.setSelection(mNlpText.getText().length());
							
							String sub = params.optString("sub");
							if ("nlp".equals(sub)) {
								// 解析得到语义结果
								String resultStr = cntJson.optString("intent");
								Log.i( TAG, resultStr );
							}
						}
					} catch (Throwable e) {
						e.printStackTrace();
						mNlpText.append( "\n" );
						mNlpText.append( e.getLocalizedMessage());
					}
					
					mNlpText.append( "\n" );
				} break;
	
				case AIUIConstant.EVENT_ERROR: {
					mNlpText.append( "\n" );
					mNlpText.append( "错误: " + event.arg1+"\n" + event.info );
				} break;
	
				case AIUIConstant.EVENT_VAD: {
					if (AIUIConstant.VAD_BOS == event.arg1) {
						showTip("找到vad_bos");
					} else if (AIUIConstant.VAD_EOS == event.arg1) {
						showTip("找到vad_eos");
					} else {
						showTip("" + event.arg2);
					}
				} break;
				
				case AIUIConstant.EVENT_START_RECORD: {
					showTip("已开始录音");
				} break;
				
				case AIUIConstant.EVENT_STOP_RECORD: {
					showTip("已停止录音");
				} break;
	
				case AIUIConstant.EVENT_STATE: {	// 状态事件
					mAIUIState = event.arg1;
					
					if (AIUIConstant.STATE_IDLE == mAIUIState) {
						// 闲置状态，AIUI未开启
						showTip("STATE_IDLE");
					} else if (AIUIConstant.STATE_READY == mAIUIState) {
						// AIUI已就绪，等待唤醒
						showTip("STATE_READY");
					} else if (AIUIConstant.STATE_WORKING == mAIUIState) {
						// AIUI工作中，可进行交互
						showTip("STATE_WORKING");
					}
				} break;
				
				case AIUIConstant.EVENT_CMD_RETURN: {
					if (AIUIConstant.CMD_SYNC == event.arg1) {	// 数据同步的返回
						int dtype = event.data.getInt("sync_dtype", -1);
						int retCode = event.arg2;

						switch (dtype) {
							case AIUIConstant.SYNC_DATA_SCHEMA: {
								if (AIUIConstant.SUCCESS == retCode) {
									// 上传成功，记录上传会话的sid，以用于查询数据打包状态
									// 注：上传成功并不表示数据打包成功，打包成功与否应以同步状态查询结果为准，数据只有打包成功后才能正常使用
									mSyncSid = event.data.getString("sid");

									// 获取上传调用时设置的自定义tag
									String tag = event.data.getString("tag");

									// 获取上传调用耗时，单位：ms
									long timeSpent = event.data.getLong("time_spent", -1);
									if (-1 != timeSpent) {
										mTimeSpentText.setText(timeSpent + "ms");
									}

									showTip("上传成功，sid=" + mSyncSid + "，tag=" + tag + "，你可以试着说“打电话给刘德华”");
								} else {
									mSyncSid = "";
									showTip("上传失败，错误码：" + retCode);
								}
							} break;
						}
					} else if (AIUIConstant.CMD_QUERY_SYNC_STATUS == event.arg1) {	// 数据同步状态查询的返回
						// 获取同步类型
						int syncType = event.data.getInt("sync_dtype", -1);
						if (AIUIConstant.SYNC_DATA_QUERY == syncType) {
							// 若是同步数据查询，则获取查询结果，结果中error字段为0则表示上传数据打包成功，否则为错误码
							String result = event.data.getString("result");

							showTip(result);
						}
					}
				} break;
				
				default:
					break;
			}
		}

	};
 
    @Override
    protected void onDestroy() {
    	super.onDestroy();
        
    	if( null != mAIUIAgent ){
    		mAIUIAgent.destroy();
    		mAIUIAgent = null;
    	}
    }
	
	private void showTip(final String str) {
		runOnUiThread(new Runnable() {
			
			@Override
			public void run() {
				mToast.setText(str);
				mToast.show();
			}
		});
	}
}
