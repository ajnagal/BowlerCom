/*******************************************************************************
 * Copyright 2010 Neuron Robotics, LLC
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
package com.neuronrobotics.sdk.dyio;

import java.util.ArrayList;
import java.util.Collection;

import com.neuronrobotics.sdk.commands.bcs.io.GetChannelModeCommand;
import com.neuronrobotics.sdk.commands.bcs.io.GetValueCommand;
import com.neuronrobotics.sdk.commands.bcs.io.SetChannelValueCommand;
import com.neuronrobotics.sdk.commands.bcs.io.setmode.SetChannelModeCommand;
import com.neuronrobotics.sdk.common.BowlerAbstractCommand;
import com.neuronrobotics.sdk.common.BowlerDatagram;
import com.neuronrobotics.sdk.common.ByteList;
import com.neuronrobotics.sdk.common.ISendable;
import com.neuronrobotics.sdk.common.InvalidResponseException;
import com.neuronrobotics.sdk.common.Log;
import com.neuronrobotics.sdk.dyio.peripherals.DyIOAbstractPeripheral;
/**
 * A DyIO channel. This represents a single DyIO pin.
 * @author rbreznak
 *
 */
public class DyIOChannel implements IDyIOChannel {
	private int MAXATTEMPTS = 3;
	
	private DyIO device;
	private int number;
	private boolean editable;
	private DyIOChannelMode current;
	private boolean isAsync=false;
	private ArrayList<IChannelEventListener> listeners = new ArrayList<IChannelEventListener>();
	
	protected int cachedValue = 0;
	private boolean cachedMode=false;
	private DyIOAbstractPeripheral dap=null;
	/**
	 * Construct a channel object.
	 * @param dyio			The DyIO that the channel belongs on
	 * @param channel		The Channel on the DyIO that this object corresponds to.
	 * @param mode			The Type of channel.
	 * @param isEditable	Lock the channel parameters after creation.
	 */
	public DyIOChannel(DyIO dyio, int channel, DyIOChannelMode mode, boolean isEditable) {
		setDevice(dyio);
		number = channel;
		current = mode;
		editable = isEditable;
	}
	
	/**
	 * 
	 * 
	 * @param dyio
	 * @param channel
	 * @param mode
	 * @param isEditable
	 */
	public void update(DyIO dyio, int channel, DyIOChannelMode mode, boolean isEditable) {
		setDevice(dyio);
		number = channel;
		current = mode;
		editable = isEditable;
	}
	
	/**
	 * Get the channel's number.
	 * @return The Channel on the corresponding DyIO that this pin belongs to.
	 */
	public int getNumber() {
		return number;
	}
	
	/**
	 * Can the parameters of this channel be edited?.
	 * 
	 * @return True if the parameters can be changed
	 */
	public boolean isEditable() {
		return editable;
	}
	
	/**
	 * Send a command to the Channel's DyIO.
	 * @param command The command to send.
	 */
	public void send(BowlerAbstractCommand command) {
		getDevice().send(command);
	}
	/**
	 * Clear list of objects that have subscribed to channel updates.
	 */
	public void removeAllChannelEventListeners() {
		listeners.clear();
	}
	
	/**
	 * Remove a particular subscription.
	 * 
	 * @param l
	 *            The object that has subscribed to updates
	 */
	public void removeChannelEventListener(IChannelEventListener l) {
		if(!listeners.contains(l)) {
			return;
		}
		
		listeners.remove(l);
	}
	
	/**
	 * Add an object that wishes to receive channel updates.
	 * 
	 * @param l
	 *            The object that wishes to receive updates.
	 */
	public void addChannelEventListener(IChannelEventListener l) {
		if(listeners.contains(l)) {
			return;
		}
		
		listeners.add(l);
	}
	
	/**
	 * Set the mode of the DyIO Channel.
	 * 
	 * @param mode
	 *            The desired channel mode.
	 * @return True if successful
	 */
	public boolean setMode(DyIOChannelMode mode) {
		return setMode(mode, false);
	}
	
	/**
	 * Gets the mode of the channel. If resync is true, then the channel will do
	 * a live query to the device for its current mode and cache it to the
	 * channel for future use.
	 * 
	 * Identical to calling <code>resync(false)</code> before
	 * <code>getMode()</code>
	 * 
	 * @param resync
	 * @return
	 */
	public DyIOChannelMode getMode(boolean resync) {
		resync(false);
		return getMode();
	}
	
	private void setDevice(DyIO device) {
		this.device = device;
	}

	/**
	 * 
	 * 
	 * @return
	 */
	public DyIO getDevice() {
		return device;
	}
	
	/**
	 * Live query the device for its mode and cache it.
	 * 
	 * @param all - should all channels be refreshed.
	 */
	public void resync(boolean all) {
		if(all) {
			getDevice().resync();
			return;
		}
		
		BowlerDatagram bd = getDevice().send(new GetChannelModeCommand(number));
		current = DyIOChannelMode.get(bd.getData().getByte(0));
	}
	
	public boolean canBeMode(DyIOChannelMode m) {
		Collection<DyIOChannelMode> modes = getAvailableModes();
		for(DyIOChannelMode mo:modes) {
			if(mo == m)
				return true;
		}
		return false;
	}
	
	public Collection<DyIOChannelMode> getAvailableModes() {
		Collection<DyIOChannelMode> modes = new ArrayList<DyIOChannelMode>();
		modes.add(DyIOChannelMode.DIGITAL_IN);
		modes.add(DyIOChannelMode.DIGITAL_OUT);
		
		if(number < 12 && device.getBankAState() != DyIOPowerState.REGULATED) {
			modes.add(DyIOChannelMode.SERVO_OUT);	
		}

		if(number > 11 && device.getBankBState() != DyIOPowerState.REGULATED) {
			modes.add(DyIOChannelMode.SERVO_OUT);	
		}
		
		
		
		switch(number) {
		case 0:
			modes.add(DyIOChannelMode.SPI_CLOCK);
			modes.add(DyIOChannelMode.COUNT_IN_HOME);
			modes.add(DyIOChannelMode.COUNT_OUT_HOME);
			break;
		case 1:
			modes.add(DyIOChannelMode.SPI_MISO);
			modes.add(DyIOChannelMode.COUNT_IN_HOME);
			modes.add(DyIOChannelMode.COUNT_OUT_HOME);
			break;
		case 2:
			modes.add(DyIOChannelMode.SPI_MOSI);
			modes.add(DyIOChannelMode.COUNT_IN_HOME);
			modes.add(DyIOChannelMode.COUNT_OUT_HOME);
			break;
		case 3:
			modes.add(DyIOChannelMode.COUNT_IN_HOME);
			modes.add(DyIOChannelMode.COUNT_OUT_HOME);
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			modes.add(DyIOChannelMode.PWM_OUT);
			modes.add(DyIOChannelMode.DC_MOTOR_VEL);
			break;
		case 8:
		case 9:
		case 10:
		case 11:
			modes.add(DyIOChannelMode.DC_MOTOR_DIR);
		case 12:
		case 13:
		case 14:
		case 15:
			modes.add(DyIOChannelMode.ANALOG_IN);
			break;
		case 16:
			modes.add(DyIOChannelMode.USART_TX);
			modes.add(DyIOChannelMode.COUNT_IN_DIR);
			modes.add(DyIOChannelMode.COUNT_OUT_DIR);
			break;
		case 17:
			modes.add(DyIOChannelMode.USART_RX);
			modes.add(DyIOChannelMode.COUNT_IN_INT);
			modes.add(DyIOChannelMode.COUNT_OUT_INT);
			break;
		case 18:
		case 20:
		case 22:
			modes.add(DyIOChannelMode.COUNT_IN_DIR);
			modes.add(DyIOChannelMode.COUNT_OUT_DIR);
			break;
		case 19:
		case 21:
		case 23:
			modes.add(DyIOChannelMode.COUNT_IN_INT);
			modes.add(DyIOChannelMode.COUNT_OUT_INT);
			break;
		}
		if(number == 23) {
			modes.add(DyIOChannelMode.PPM_IN);
		}
		return modes;
	}
	
	/**
	 * 
	 * 
	 * @param e
	 */
	protected void fireChannelEvent(DyIOChannelEvent e) {
		for(IChannelEventListener l : listeners) {
			l.onChannelEvent(e);
		}
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return String.format("(%02d) - %-20s", getNumber(), getMode());
	}
	
	/* (non-Javadoc)
	 * @see com.neuronrobotics.sdk.dyio.IDyIOChannel#getChannel()
	 */
	@Override
	public DyIOChannel getChannel() {
		return this;
	}
	
	/* (non-Javadoc)
	 * @see com.neuronrobotics.sdk.dyio.IDyIOChannel#getMode()
	 */
	@Override
	public DyIOChannelMode getMode() {		
		return current;
	}
	
	/* (non-Javadoc)
	 * @see com.neuronrobotics.sdk.dyio.IDyIOChannel#getValue()
	 */
	@Override
	public int getValue() {
		BowlerDatagram response=null;
		try {
			response = getDevice().send(new GetValueCommand(number));
		} catch (InvalidResponseException e) {
			response = getDevice().send(new GetValueCommand(number));
		}
		switch(current) {
		case ANALOG_IN:
		case DIGITAL_IN:
		case DIGITAL_OUT:
		case SERVO_OUT: 
		case PWM_OUT:
			return ByteList.convertToInt(response.getData().getBytes(1), false);
		case COUNT_IN_INT:
		case COUNT_IN_DIR:
		case COUNT_IN_HOME:
		case COUNT_OUT_INT:
		case COUNT_OUT_DIR:
		case COUNT_OUT_HOME:
			byte [] b=response.getData().getBytes(1);
			int val = ByteList.convertToInt(b, true);
			return val;
		default:
			// fail gracefully
			return 0;
		} 
	}
	
	/* (non-Javadoc)
	 * @see com.neuronrobotics.sdk.dyio.IDyIOChannel#setMode(com.neuronrobotics.sdk.dyio.DyIOChannelMode, boolean)
	 */
	@Override
	public boolean setMode(DyIOChannelMode mode, boolean async) {
		if (getMode() == mode && (async == isAsync))
			return true;
		for(int i = 0; i < MAXATTEMPTS; i++) {
			try {
				isAsync = async;
				current = mode;
				getDevice().send(new SetChannelModeCommand(number, mode, async));
				getDevice().resync();
				return true;
			} catch (InvalidResponseException e) {
				Log.error(e.getMessage());
				try {
					Thread.sleep(100);
				} catch (InterruptedException e1) {
					return false;
				}
			}
		}
		return false;
	}
	
	/* (non-Javadoc)
	 * @see com.neuronrobotics.sdk.dyio.IDyIOChannel#setValue(int)
	 */
	@Override
	public boolean setValue(int value) {
		setCachedValue(value);
		if(cachedMode)
			return true;
		if(dap!=null)
			return dap.flush();
		return flush();
	}
	
	public boolean flush() {
		ByteList b = new ByteList();
		switch(getMode()){
		case COUNT_IN_INT:
		case COUNT_IN_DIR:
		case COUNT_IN_HOME:
		case COUNT_OUT_INT:
		case COUNT_OUT_DIR:
		case COUNT_OUT_HOME:
			b.addAs32(getCachedValue());
		default:
			b.add(getCachedValue());
		}
		return setValue(b);
	}

	/* (non-Javadoc)
	 * @see com.neuronrobotics.sdk.dyio.IDyIOChannel#setValue(com.neuronrobotics.sdk.common.ISendable)
	 */
	@Override
	public boolean setValue(ISendable data) {
		int attempts = MAXATTEMPTS;
		if(getMode() == DyIOChannelMode.USART_RX ||getMode() == DyIOChannelMode.USART_TX )
			attempts=1;
		for(int i = 0; i < attempts; i++) {
			try {
				getDevice().send(new SetChannelValueCommand(number, data));
				return true;
			} catch (InvalidResponseException e) {
				try {
					Thread.sleep(100);
				} catch (InterruptedException e1) {
					return false;
				}
			}
		}
		
		return false;
	}

	public void setCachedValue(int cachedValue) {
		this.cachedValue = cachedValue;
	}

	public int getCachedValue() {
		return cachedValue;
	}
	
	public boolean getCachedMode() {
		return cachedMode;
	}

	public void setCachedMode(boolean mode) {
		cachedMode=mode;
	}

	public void setDap(DyIOAbstractPeripheral dap) {
		this.dap = dap;
	}

	public DyIOAbstractPeripheral getDap() {
		return dap;
	}	
}