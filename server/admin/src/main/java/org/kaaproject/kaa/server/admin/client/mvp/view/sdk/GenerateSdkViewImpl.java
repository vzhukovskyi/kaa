/*
 * Copyright 2014-2015 CyberVision, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.kaaproject.kaa.server.admin.client.mvp.view.sdk;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.kaaproject.kaa.common.dto.SchemaDto;
import org.kaaproject.kaa.common.dto.admin.SdkPlatform;
import org.kaaproject.kaa.common.dto.event.AefMapInfoDto;
import org.kaaproject.kaa.common.dto.user.UserVerifierDto;
import org.kaaproject.kaa.server.admin.client.mvp.view.GenerateSdkView;
import org.kaaproject.kaa.server.admin.client.mvp.view.base.BaseDetailsViewImpl;
import org.kaaproject.kaa.server.admin.client.mvp.view.widget.MultiAefMapListBox;
import org.kaaproject.kaa.server.admin.client.mvp.view.widget.MultiValueListBox;
import org.kaaproject.kaa.server.admin.client.mvp.view.widget.SchemaListBox;
import org.kaaproject.kaa.server.admin.client.util.Utils;

import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.logical.shared.ValueChangeEvent;
import com.google.gwt.event.logical.shared.ValueChangeHandler;
import com.google.gwt.text.shared.Renderer;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.DisclosurePanel;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HasHorizontalAlignment;
import com.google.gwt.user.client.ui.HasVerticalAlignment;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.ValueListBox;
import com.google.gwt.user.client.ui.VerticalPanel;
import com.google.gwt.user.client.ui.Widget;

public class GenerateSdkViewImpl extends BaseDetailsViewImpl implements GenerateSdkView, ValueChangeHandler<SchemaDto>  {
    
    private static final String REQUIRED = Utils.avroUiStyle.requiredField();

    private SchemaListBox configurationSchemaVersion;
    private SchemaListBox profileSchemaVersion;
    private SchemaListBox notificationSchemaVersion;
    private SchemaListBox logSchemaVersion;
    private ValueListBox<SdkPlatform> targetPlatform;

    private List<AefMapInfoDto> aefMaps;
    private AefMapInfoDtoComparator aefMapComparator = new AefMapInfoDtoComparator();

    private MultiAefMapListBox availableAefMaps;
    private MultiAefMapListBox selectedAefMaps;

    private Button addAefMapButton;
    private Button removeAefMapButton;
    
    private ValueListBox<UserVerifierDto> defaultUserVerifier;

    
    public GenerateSdkViewImpl() {
        super(true);
    }
    
    @Override
    protected void initDetailsTable() {
        
        detailsTable.getColumnFormatter().setWidth(0, "250px");
        detailsTable.getColumnFormatter().setWidth(1, "500px");
        
        int row = 0;
        Widget label = new Label(Utils.constants.configurationSchemaVersion());
        label.addStyleName(REQUIRED);
        configurationSchemaVersion = new SchemaListBox();
        configurationSchemaVersion.setWidth("80px");
        configurationSchemaVersion.addValueChangeHandler(this);
        detailsTable.setWidget(row, 0, label);
        detailsTable.setWidget(row, 1, configurationSchemaVersion);
        
        row++;
        label = new Label(Utils.constants.profileSchemaVersion());
        label.addStyleName(REQUIRED);
        profileSchemaVersion = new SchemaListBox();
        profileSchemaVersion.setWidth("80px");
        profileSchemaVersion.addValueChangeHandler(this);
        detailsTable.setWidget(row, 0, label);
        detailsTable.setWidget(row, 1, profileSchemaVersion);
        
        row++;
        label = new Label(Utils.constants.notificationSchemaVersion());
        label.addStyleName(REQUIRED);
        notificationSchemaVersion = new SchemaListBox();
        notificationSchemaVersion.setWidth("80px");
        notificationSchemaVersion.addValueChangeHandler(this);
        detailsTable.setWidget(row, 0, label);
        detailsTable.setWidget(row, 1, notificationSchemaVersion);
        
        row++;
        label = new Label(Utils.constants.logSchemaVersion());
        label.addStyleName(REQUIRED);
        logSchemaVersion = new SchemaListBox();
        logSchemaVersion.setWidth("80px");
        logSchemaVersion.addValueChangeHandler(this);
        detailsTable.setWidget(row, 0, label);
        detailsTable.setWidget(row, 1, logSchemaVersion);
        
        row++;
        label = new Label(Utils.constants.targetPlatform());
        label.addStyleName(REQUIRED);
        Renderer<SdkPlatform> targetPlatformRenderer = new Renderer<SdkPlatform>() {
            @Override
            public String render(SdkPlatform object) {
                if (object != null) {
                    return Utils.constants.getString(object.getResourceKey());
                } else {
                    return "";
                }
            }

            @Override
            public void render(SdkPlatform object, Appendable appendable) throws IOException {
                appendable.append(render(object));
            }
        };
        targetPlatform = new ValueListBox<>(targetPlatformRenderer);
        targetPlatform.addValueChangeHandler(new ValueChangeHandler<SdkPlatform>() {
            @Override
            public void onValueChange(ValueChangeEvent<SdkPlatform> event) {
                fireChanged();
            }
        });
        detailsTable.setWidget(row, 0, label);
        detailsTable.setWidget(row, 1, targetPlatform);
        
        row++;
        FlexTable ecfsTable = new FlexTable();
        ecfsTable.setCellSpacing(6);

        availableAefMaps = new MultiAefMapListBox();
        selectedAefMaps = new MultiAefMapListBox();

        addAefMapButton = new Button(Utils.constants.add());
        removeAefMapButton = new Button(Utils.constants.remove());

        VerticalPanel availableEcfsPanel = new VerticalPanel();
        availableEcfsPanel.setSpacing(6);
        Label availableLabel = new Label(Utils.constants.available());
        availableEcfsPanel.add(availableLabel);
        availableEcfsPanel.add(availableAefMaps);

        VerticalPanel ecfButtonsPanel = new VerticalPanel();
        ecfButtonsPanel.setSpacing(6);
        ecfButtonsPanel.setVerticalAlignment(HasVerticalAlignment.ALIGN_MIDDLE);
        ecfButtonsPanel.setHorizontalAlignment(HasHorizontalAlignment.ALIGN_CENTER);
        ecfButtonsPanel.add(addAefMapButton);
        ecfButtonsPanel.add(removeAefMapButton);

        VerticalPanel selectedEcfsPanel = new VerticalPanel();
        selectedEcfsPanel.setSpacing(6);
        Label selectedLabel = new Label(Utils.constants.selected());
        selectedEcfsPanel.add(selectedLabel);
        selectedEcfsPanel.add(selectedAefMaps);

        ecfsTable.setWidget(0, 0, availableEcfsPanel);
        ecfsTable.setWidget(0, 1, ecfButtonsPanel);
        ecfsTable.setWidget(0, 2, selectedEcfsPanel);
        
        ecfsTable.getFlexCellFormatter().setVerticalAlignment(0, 1, HasVerticalAlignment.ALIGN_MIDDLE);

        DisclosurePanel ecfsDisclosure = new DisclosurePanel(Utils.constants.ecfs());
        ecfsDisclosure.setAnimationEnabled(true);
        ecfsDisclosure.setContent(ecfsTable);
        
        addAefMapButton.addStyleName(Utils.kaaAdminStyle.bAppButtonSmall());
        removeAefMapButton.addStyleName(Utils.kaaAdminStyle.bAppButtonSmall());

        addAefMapButton.addClickHandler(new ClickHandler() {
            @Override
            public void onClick(ClickEvent event) {
                addAefMap();
            }
        });

        removeAefMapButton.addClickHandler(new ClickHandler() {
            @Override
            public void onClick(ClickEvent event) {
                removeAefMap();
            }
        });

        availableAefMaps.setSize("250px", "100px");
        selectedAefMaps.setSize("250px", "100px");

        availableAefMaps.addValueChangeHandler(new ValueChangeHandler<List<AefMapInfoDto>>() {
            @Override
            public void onValueChange(ValueChangeEvent<List<AefMapInfoDto>> event) {
                updateAefMapButtons();
            }
        });

        selectedAefMaps.addValueChangeHandler(new ValueChangeHandler<List<AefMapInfoDto>>() {
            @Override
            public void onValueChange(ValueChangeEvent<List<AefMapInfoDto>> event) {
                updateAefMapButtons();
            }
        });
        detailsTable.setWidget(row, 0, ecfsDisclosure);
        detailsTable.getFlexCellFormatter().setColSpan(row, 0, 2);
        
        row++;
        label = new Label(Utils.constants.defaultUserVerifier());
        Renderer<UserVerifierDto> userVerifierRenderer = new Renderer<UserVerifierDto>() {
            @Override
            public String render(UserVerifierDto object) {
                if (object != null) {
                    return object.getName();
                } else {
                    return "";
                }
            }

            @Override
            public void render(UserVerifierDto object, Appendable appendable) throws IOException {
                appendable.append(render(object));
            }
        };
        defaultUserVerifier = new ValueListBox<>(userVerifierRenderer);
        defaultUserVerifier.addValueChangeHandler(new ValueChangeHandler<UserVerifierDto>() {
            @Override
            public void onValueChange(ValueChangeEvent<UserVerifierDto> event) {
                fireChanged();
            }
        });
        detailsTable.setWidget(row, 0, label);
        detailsTable.setWidget(row, 1, defaultUserVerifier);
    }
    
    @Override
    public void onValueChange(ValueChangeEvent<SchemaDto> event) {
        fireChanged();
    }
    
    @Override
    protected String getCreateTitle() {
        return Utils.constants.generateSdk();
    }

    @Override
    protected String getViewTitle() {
        return Utils.constants.generateSdk();
    }

    @Override
    protected String getSubTitle() {
        return Utils.constants.sdkDetails();
    }

    @Override
    public ValueListBox<SchemaDto> getConfigurationSchemaVersion() {
        return configurationSchemaVersion;
    }

    @Override
    public ValueListBox<SchemaDto> getProfileSchemaVersion() {
        return profileSchemaVersion;
    }

    @Override
    public ValueListBox<SchemaDto> getNotificationSchemaVersion() {
        return notificationSchemaVersion;
    }

    @Override
    public ValueListBox<SchemaDto> getLogSchemaVersion() {
        return logSchemaVersion;
    }

    @Override
    public ValueListBox<SdkPlatform> getTargetPlatform() {
        return targetPlatform;
    }

    @Override
    public MultiValueListBox<AefMapInfoDto> getSelectedAefMaps() {
        return selectedAefMaps;
    }

    @Override
    public ValueListBox<UserVerifierDto> getDefaultUserVerifier() {
        return defaultUserVerifier;
    }
    
    @Override
    public void setAefMaps(List<AefMapInfoDto> aefMaps) {
        this.aefMaps = aefMaps;
        Collections.sort(this.aefMaps, aefMapComparator);
        availableAefMaps.setAcceptableValues(aefMaps);
        
    }

    @Override
    protected void resetImpl() {
        configurationSchemaVersion.reset();
        profileSchemaVersion.reset();
        notificationSchemaVersion.reset();
        logSchemaVersion.reset();
        targetPlatform.setValue(SdkPlatform.ANDROID);
        targetPlatform.setAcceptableValues(Arrays.asList(SdkPlatform.values()));
        availableAefMaps.reset();
        selectedAefMaps.reset();
        addAefMapButton.setEnabled(false);
        removeAefMapButton.setEnabled(false);
        defaultUserVerifier.setValue(null);
        defaultUserVerifier.setAcceptableValues(Collections.<UserVerifierDto>emptyList());
    }

    @Override
    protected boolean validate() {
        boolean result = configurationSchemaVersion.getValue() != null;
        result &= profileSchemaVersion.getValue() != null;
        result &= notificationSchemaVersion.getValue() != null;
        result &= logSchemaVersion.getValue() != null;
        result &= targetPlatform.getValue() != null;
        return result;
    }
    
    private void addAefMap() {
        List<AefMapInfoDto> selected = availableAefMaps.getValue();
        availableAefMaps.setValue(null, true);
        Map<String, AefMapInfoDto> idMap = new HashMap<String, AefMapInfoDto>();
        for (AefMapInfoDto aefMap : selected) {
            AefMapInfoDto previous = idMap.get(aefMap.getEcfId());
            if (previous == null || aefMap.getVersion() > previous.getVersion()) {
                idMap.put(aefMap.getEcfId(), aefMap);
            }
        }

        List<AefMapInfoDto> totalSelected = new ArrayList<>();
        totalSelected.addAll(idMap.values());
        totalSelected.addAll(selectedAefMaps.getValues());
        updateAefMapLists(totalSelected);
    }

    private void removeAefMap() {
        List<AefMapInfoDto> selected = selectedAefMaps.getValue();
        selectedAefMaps.setValue(null, true);
        List<AefMapInfoDto> totalSelected = new ArrayList<>();
        for (AefMapInfoDto aefMap : selectedAefMaps.getValues()) {
            if (!selected.contains(aefMap)) {
                totalSelected.add(aefMap);
            }
        }
        updateAefMapLists(totalSelected);
    }

    private void updateAefMapLists(List<AefMapInfoDto> totalSelected) {
        List<AefMapInfoDto> available = new ArrayList<>();
        Map<String, AefMapInfoDto> idMap = new HashMap<String, AefMapInfoDto>();
        for (AefMapInfoDto aefMap : totalSelected) {
            idMap.put(aefMap.getEcfId(), aefMap);
        }
        for (AefMapInfoDto aefMap : aefMaps) {
            if (!idMap.containsKey(aefMap.getEcfId())) {
                available.add(aefMap);
            }
        }
        Collections.sort(available, aefMapComparator);
        Collections.sort(totalSelected, aefMapComparator);
        availableAefMaps.setAcceptableValues(available);
        selectedAefMaps.setAcceptableValues(totalSelected);
    }
    
    private void updateAefMapButtons() {
        boolean availableSelected = availableAefMaps.getValue() != null && !availableAefMaps.getValue().isEmpty();
        boolean selectedSelected = selectedAefMaps.getValue() != null && !selectedAefMaps.getValue().isEmpty();
        addAefMapButton.setEnabled(availableSelected);
        removeAefMapButton.setEnabled(selectedSelected);
    }

    class AefMapInfoDtoComparator implements Comparator<AefMapInfoDto> {
        @Override
        public int compare(AefMapInfoDto o1, AefMapInfoDto o2) {
            int result = o1.getEcfName().compareTo(o2.getEcfName());
            if (result == 0) {
                result = o1.getVersion() - o2.getVersion();
            }
            return result;
        }
    }

    @Override
    protected void updateSaveButton(boolean enabled, boolean invalid) {
        saveButton.setText(Utils.constants.generateSdk());
        saveButton.setEnabled(!invalid);
    }

}
